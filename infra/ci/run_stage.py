#!/usr/bin/env python3
"""Run target phases until completion or a planned deadline."""

import argparse
import codecs
import os
from pathlib import Path
import shutil
import signal
import subprocess
import sys
import time

if os.name != "nt":
    import resource

from common import (ROOT, build_environment, configuration, output, summary,
                    validate_manifest, write_json)


class Cancelled(Exception):
    pass


def cancel(signum, frame):
    raise Cancelled(f"received signal {signum}")


def group_alive(pid):
    try:
        os.killpg(pid, 0)
        return True
    except ProcessLookupError:
        return False


def stop_group(process, grace=120):
    """Do not archive until every process in our build group has stopped."""
    if group_alive(process.pid):
        try:
            os.killpg(process.pid, signal.SIGINT)
        except ProcessLookupError:
            # The last process may exit between the existence check and signal.
            pass
    until = time.monotonic() + grace
    while group_alive(process.pid) and time.monotonic() < until:
        process.poll()  # Reap the group leader, too.
        time.sleep(0.2)
    if group_alive(process.pid):
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            return process.wait()
        process.wait()
        raise RuntimeError("build did not stop cleanly; keeping the previous checkpoint")
    return process.wait()


def forward_log(stream, decoder, *, final=False):
    """Mirror saved output without waiting for a newline or a child pipe."""
    while True:
        data = stream.read(64 * 1024)
        text = decoder.decode(data, final=final and not data)
        if text:
            sys.stdout.write(text)
            sys.stdout.flush()
        # Bound each live read so log traffic cannot starve the deadline check.
        # Once the process group stops, drain the remaining bytes and decoder.
        if not final or not data:
            return


def run_command(command, cwd, env, deadline, log_path):
    if os.name == "nt":
        from windows_process import run_command as run_windows_command
        return run_windows_command(command, cwd, env, deadline, log_path, forward_log)
    with log_path.open("wb") as log, log_path.open("rb") as live_log:
        decoder = codecs.getincrementaldecoder("utf-8")(errors="replace")
        # Keep the compiler writing directly to disk, including during SIGINT
        # shutdown, so console forwarding cannot fill a child-output pipe.
        process = subprocess.Popen(command, cwd=cwd, env=env, start_new_session=True,
                                   stdout=log, stderr=subprocess.STDOUT)
        heartbeat = time.monotonic()
        try:
            while process.poll() is None:
                forward_log(live_log, decoder)
                if time.monotonic() - heartbeat >= 60:
                    print(f"Building; {max(0, deadline - time.monotonic()) / 60:.0f} minutes left in this stage", flush=True)
                    heartbeat = time.monotonic()
                if time.monotonic() >= deadline:
                    # A real failure already visible at the boundary remains a failure.
                    if process.poll() is not None:
                        break
                    code = stop_group(process)
                    if code == 0:
                        return "complete", code
                    if code not in (-signal.SIGINT, 128 + signal.SIGINT):
                        raise subprocess.CalledProcessError(code, command)
                    return "checkpointed", code
                time.sleep(0.5)
            code = process.wait()
            if code:
                raise subprocess.CalledProcessError(code, command)
            if group_alive(process.pid):
                stop_group(process)
                raise RuntimeError("build exited with live child processes")
            return "complete", code
        except BaseException:
            if group_alive(process.pid):
                stop_group(process)
            raise
        finally:
            forward_log(live_log, decoder, final=True)


def completed_actions(out):
    log = out / ".ninja_log"
    if not log.is_file():
        return 0
    with log.open(errors="replace") as stream:
        return sum(1 for line in stream if line and not line.startswith("#"))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work-root", required=True, type=Path)
    parser.add_argument("--minutes", type=int)
    parser.add_argument("--jobs", type=int)
    args = parser.parse_args()
    work = args.work_root.resolve()
    _, profile = configuration()
    jobs = args.jobs if args.jobs is not None else profile["jobs"]
    minutes = args.minutes if args.minutes is not None else profile["compile_minutes"]
    if not 1 <= jobs <= 8 or not 10 <= minutes <= 240:
        raise ValueError("jobs must be 1..8 and compile minutes 10..240")
    # Include checkout, tool installation and restore in the job budget.
    job_start = float(os.environ.get("THORIUM_JOB_STARTED", time.time()))
    seconds = min(minutes * 60, 350 * 60 - (time.time() - job_start) - 75 * 60)
    if seconds < 60:
        raise RuntimeError("restore consumed the compile window; previous checkpoint remains valid")
    manifest = validate_manifest(work)
    if manifest["targets"] != profile["targets"] or manifest["status"] != "checkpointed":
        raise ValueError("invalid phase list or checkpoint state")
    phase = manifest["phase_index"]
    if not 0 <= phase < len(profile["targets"]):
        raise ValueError("invalid phase index")
    previous_stage = manifest["stage"]
    manifest["stage"] += 1
    if previous_stage:
        manifest["resumed_stages"] += 1
    logs = work / "logs"
    logs.mkdir(exist_ok=True)
    env = build_environment(work)
    out = work / "src" / profile["out_dir"]
    before = completed_actions(out)
    start = time.monotonic()
    deadline = start + seconds
    signal.signal(signal.SIGTERM, cancel)
    signal.signal(signal.SIGINT, cancel)
    manifest["status"] = "running"
    write_json(work / "manifest.json", manifest)
    try:
        while phase < len(profile["targets"]):
            target = profile["targets"][phase]
            command = [sys.executable, str(ROOT / "build.py"), "--chromium-src",
                       str(work / "src"), "--expect-os", profile["target_os"],
                       "--expect-cpu", profile["target_cpu"],
                       "--target", target, "-j", str(jobs)]
            print(f"Stage {manifest['stage']}, phase {phase + 1}: {target}", flush=True)
            log_path = logs / f"stage-{manifest['stage']:02d}-phase-{phase + 1}.log"
            status, code = run_command(command, work / "src", env, deadline, log_path)
            manifest["last_exit_code"] = code
            if status == "checkpointed":
                break
            phase += 1
            manifest["phase_index"] = phase
            if time.monotonic() >= deadline and phase < len(profile["targets"]):
                break
        manifest["status"] = "complete" if phase == len(profile["targets"]) else "checkpointed"
    except BaseException as error:
        manifest["status"] = "cancelled" if isinstance(error, (Cancelled, KeyboardInterrupt)) else "failed"
        manifest["error"] = str(error)
        if isinstance(error, subprocess.CalledProcessError):
            manifest["last_exit_code"] = error.returncode
        if 'log_path' in locals() and log_path.is_file():
            # A bounded tail keeps compiler diagnostics visible in Actions.
            with log_path.open('rb') as stream:
                stream.seek(max(0, log_path.stat().st_size - 16000))
                print(stream.read().decode(errors='replace'), flush=True)
        raise
    finally:
        manifest["compile_seconds"] = time.monotonic() - start
        manifest["completed_actions_delta"] = completed_actions(out) - before
        manifest["max_rss_kib"] = (resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
                                   if os.name != "nt" else None)
        if sys.platform == "darwin":
            manifest["max_rss_kib"] //= 1024  # Darwin reports bytes, Linux reports KiB.
            manifest["backing_free_bytes"] = shutil.disk_usage(os.environ["RUNNER_TEMP"]).free
        manifest["free_bytes"] = shutil.disk_usage(work).free
        write_json(work / "manifest.json", manifest)
        summary(f"Stage {manifest['stage']}: **{manifest['status']}**, "
                f"completed phases: {phase}/{len(profile['targets'])}, "
                f"build time: {manifest['compile_seconds'] / 60:.1f} minutes.")
    output("status", manifest["status"])
    output("stage", manifest["stage"])


if __name__ == "__main__":
    main()
