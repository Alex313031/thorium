#!/usr/bin/env python3
"""Install the locked Chromium host dependencies on a disposable Actions runner."""

import argparse
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys
import time

from common import (checked_path, configuration, run, sha256, summary,
                    validate_manifest, write_json)

SNAPSHOT = Path("host-tools/install-build-deps.py")


def save_installer(work, sources):
    """Keep the verified upstream installer before Thorium modifies the source."""
    spec = sources["host_dependencies"]
    script = checked_path(work / "src", spec["script"])
    if script.is_symlink() or not script.is_file() or sha256(script) != spec["sha256"]:
        raise ValueError("Chromium host dependency installer does not match the source lock")
    snapshot = checked_path(work, SNAPSHOT)
    if snapshot.exists():
        raise ValueError("host dependency installer snapshot already exists")
    snapshot.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(script, snapshot)


def install(work, sources, profile, label):
    if (os.environ.get("GITHUB_ACTIONS") != "true" or
            os.environ.get("RUNNER_ENVIRONMENT") != "github-hosted" or
            platform.system() != "Linux" or platform.machine() != "x86_64"):
        raise ValueError("host dependency installation requires a GitHub-hosted Linux x64 runner")
    supported = {"linux": ("x86", "x64", "arm64"), "android": ("arm", "arm64")}
    if profile["target_cpu"] not in supported.get(profile["target_os"], ()):
        raise ValueError("host dependency policy is not configured for this target")
    if profile["target_os"] == "android" and "--lib32" not in profile["host_deps_args"]:
        raise ValueError("Android builds require --lib32 host dependencies")
    spec = sources["host_dependencies"]
    src = work.resolve() / "src"
    script = checked_path(work, SNAPSHOT)
    if script.is_symlink() or not script.is_file() or sha256(script) != spec["sha256"]:
        raise ValueError("Chromium host dependency installer does not match the source lock")
    flags = profile["host_deps_args"]
    report = {"script": spec["script"], "saved_script": str(SNAPSHOT),
              "sha256": spec["sha256"], "args": flags,
              "runner_image": os.environ.get("ImageVersion", "unknown"),
              "status": "running"}
    start = time.monotonic()
    env = dict(os.environ, DEBIAN_FRONTEND="noninteractive")
    try:
        # The upstream script invokes sudo for apt and locale setup itself.
        # Run once per runner: system packages are not part of the checkpoint.
        run([sys.executable, script, *flags], cwd=src, env=env)
        run(["sudo", "-n", "apt-get", "clean"], env=env)
        report["installed_packages"] = subprocess.check_output(
            ["dpkg-query", "-W", "-f=${binary:Package}\t${Version}\n"],
            text=True).splitlines()
        report["status"] = "complete"
    except BaseException as error:
        report["status"] = "failed"
        report["error"] = str(error)
        if isinstance(error, subprocess.CalledProcessError):
            report["exit_code"] = error.returncode
        raise
    finally:
        report["seconds"] = time.monotonic() - start
        disk = os.statvfs(work)
        report["free_bytes"] = disk.f_bavail * disk.f_frsize
        write_json(work / "logs" / f"host-deps-{label}.json", report)
        summary(f"Chromium host dependencies: **{report['status']}**, "
                f"{report['seconds'] / 60:.1f} minutes.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work-root", required=True, type=Path)
    args = parser.parse_args()
    work = args.work_root.resolve()
    manifest = validate_manifest(work)
    if manifest["status"] != "checkpointed":
        raise ValueError("host dependency setup requires a validated build checkpoint")
    sources, profile = configuration()
    install(work, sources, profile, f"stage-{manifest['stage'] + 1:02d}")


if __name__ == "__main__":
    main()
