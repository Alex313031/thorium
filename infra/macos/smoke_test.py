#!/usr/bin/env python3
# Copyright (c) 2026 Alex313031 and gz83.
"""Install one DMG into a temporary directory and test it on a native Mac."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import plistlib
import re
import shutil
import signal
import subprocess
import tempfile
import time

from package_dmg import macho_files, validate_architectures


HERE = Path(__file__).resolve().parent


def write_json(path, value):
    path.write_text(json.dumps(value, indent=2) + "\n", encoding="utf-8")


def command(args, output, *, required=False, timeout=60):
    try:
        result = subprocess.run(args, capture_output=True, text=True, timeout=timeout)
        output.write_text(result.stdout + result.stderr, encoding="utf-8")
        if required and result.returncode:
            raise RuntimeError(f"Command failed ({result.returncode}): {args}; see {output}")
        return result
    except (OSError, subprocess.TimeoutExpired) as error:
        output.write_text(str(error) + "\n", encoding="utf-8")
        if required:
            raise
        return None


def validate_host(target, machine, translated):
    expected = {"arm64": "arm64", "x64": "x86_64"}[target]
    if machine != expected or translated == "1":
        raise RuntimeError(f"Requires native {expected}; host={machine}, Rosetta={translated}")


def validate_digest(dmg, expected):
    if expected and not re.fullmatch(r"[0-9a-fA-F]{64}", expected):
        raise ValueError("Expected SHA-256 must have 64 hexadecimal characters")
    with dmg.open("rb") as stream:
        digest = hashlib.file_digest(stream, "sha256").hexdigest()
    if expected and digest != expected.lower():
        raise ValueError(f"DMG SHA-256 mismatch: {digest}")
    return digest


def collect_reports(root, profile, started):
    # Dedicated Crashpad database is already under root; also retain fallbacks.
    for name in ("Crashpad", "Crash Reports"):
        candidate = profile / name
        if candidate.is_dir():
            shutil.copytree(candidate, root / f"profile-{name}", dirs_exist_ok=True)
    for index, folder in enumerate((
        Path.home() / "Library/Logs/DiagnosticReports", Path("/Library/Logs/DiagnosticReports")
    )):
        if not folder.is_dir():
            continue
        for source in folder.glob("*Thorium*"):
            if source.is_file() and source.stat().st_mtime >= started:
                destination = root / "system-reports" / str(index)
                destination.mkdir(parents=True, exist_ok=True)
                shutil.copy2(source, destination / source.name)
    return [str(p.relative_to(root)) for p in root.rglob("*")
            if p.is_file() and p.suffix.lower() in (".dmp", ".ips", ".crash")]


def run_probe(browser, work, results, version, mode, *, extra_args=()):
    results.mkdir()
    profile = work / results.name
    profile.mkdir()
    dumps = results / "crashpad"
    dumps.mkdir()
    config = {
        "browser": str(browser), "profile": str(profile), "results": str(results),
        "dumps": str(dumps), "mode": mode, "extra_args": list(extra_args),
        "expected_version": version,
        "local_url": (HERE / "probe.html").as_uri(),
    }
    write_json(results / "config.json", config)
    started = time.time()
    with (results / "harness.log").open("w") as output:
        child = subprocess.Popen(
            ["node", str(HERE / "browser_probe.mjs"), str(results / "config.json")],
            stdout=output, stderr=subprocess.STDOUT, start_new_session=True,
        )
        timed_out = False
        try:
            child.wait(timeout=300)
        except subprocess.TimeoutExpired:
            timed_out = True
            child.terminate()  # Node samples the browser and closes its own process group.
            try:
                child.wait(timeout=60)
            except subprocess.TimeoutExpired:
                child.kill()
                child.wait(timeout=10)
    report_file = results / "report.json"
    report = json.loads(report_file.read_text()) if report_file.exists() else {}
    if timed_out:
        report.update(status="FAIL", error="Probe exceeded its time limit")
        # Last resort if Node could not finish cleanup; pid was recorded at launch.
        if isinstance(report.get("pid"), int) and report["pid"] > 1:
            try:
                os.killpg(report["pid"], signal.SIGKILL)
            except ProcessLookupError:
                pass
    report["harness_exit_code"] = child.returncode
    report["crash_artifacts"] = collect_reports(results, profile, started)
    status = report.get("status")
    expected_exit = {"PASS": 0, "FAIL": 1, "INCOMPLETE": 2}
    if status not in expected_exit or child.returncode != expected_exit.get(status):
        report["status"] = "FAIL"
        report.setdefault("error", "Missing probe status or unexpected harness exit code")
    if report["crash_artifacts"]:
        report["status"] = "FAIL"
        report.setdefault("error", "Crash artifacts detected; inspect their timestamps and stacks")
    report["success"] = report["status"] == "PASS"
    write_json(report_file, report)
    return report


def summarize(probes):
    # Diagnostic successes can never turn a failed baseline into a passing test.
    statuses = [probes.get(name, {}).get("status", "INCOMPLETE") for name in ("headless", "gui")]
    if "FAIL" in statuses:
        return "FAIL"
    return "PASS" if all(status == "PASS" for status in statuses) else "INCOMPLETE"


def run_browser_probes(browser, work, results, version, report):
    def record_probe(label, mode, *, extra_args=()):
        try:
            report["probes"][label] = run_probe(
                browser, work, results / label, version, mode,
                extra_args=extra_args)
        except Exception as error:
            report["probes"][label] = {
                "success": False, "status": "FAIL", "error": str(error)}
        report["status"] = summarize(report["probes"])
        report["success"] = report["status"] == "PASS"
        write_json(results / "summary.json", report)

    for mode in ("headless", "gui"):
        record_probe(mode, mode)

    if report["probes"]["gui"]["status"] == "FAIL":
        # Repeat unchanged GUI arguments before changing any flags. run_probe
        # gives this label a separate profile and crash directory. A pass can
        # expose startup/order effects, but cannot replace the failed baseline.
        record_probe("gui-repeat", "gui")

    # Prefer headless diagnostics when both baselines fail, as in the WOA harness.
    mode = next((mode for mode in ("headless", "gui")
                 if report["probes"][mode]["status"] == "FAIL"), None)
    if mode:
        for name, flag in (("no-extensions", "--disable-extensions"),
                           ("no-gpu", "--disable-gpu"), ("no-sandbox", "--no-sandbox")):
            record_probe(f"{mode}-{name}", mode, extra_args=[flag])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dmg", type=Path, required=True)
    parser.add_argument("--arch", choices=("x64", "arm64"), required=True)
    parser.add_argument("--expected-version", required=True)
    parser.add_argument("--expected-sha256", default="")
    parser.add_argument("--results", type=Path, required=True)
    args = parser.parse_args()
    results = args.results.resolve()
    results.mkdir(parents=True, exist_ok=True)
    report = {"success": False, "status": "INCOMPLETE", "arch": args.arch, "expected_version": args.expected_version,
              "expected_macos_version": os.environ.get("TEST_MACOS_VERSION"),
              "probes": {}, "errors": [], "warnings": []}
    try:
        if platform.system() != "Darwin":
            raise RuntimeError("This harness requires macOS")
        if not re.fullmatch(r"\d+\.\d+\.\d+\.\d+", args.expected_version):
            raise ValueError("Expected version must be a four-part Chromium version")
        translated = command(["sysctl", "-n", "sysctl.proc_translated"], results / "rosetta.txt")
        validate_host(args.arch, platform.machine(), translated.stdout.strip() if translated else "")
        report["host_arch"] = platform.machine()
        report["os_version"] = platform.mac_ver()[0]
        expected_os = report["expected_macos_version"]
        if expected_os and report["os_version"].split(".")[0] != expected_os:
            raise RuntimeError(f"Expected macOS {expected_os}, got {report['os_version']}")
        report["runner_image"] = {key: os.environ.get(key) for key in ("ImageOS", "ImageVersion", "RUNNER_ARCH")}
        for name, cmd in {
            "os": ["sw_vers"], "cpu": ["sysctl", "-a"],
            "display": ["system_profiler", "SPDisplaysDataType"], "node": ["node", "--version"],
        }.items():
            completed = command(cmd, results / f"{name}.txt", required=name in ("os", "node"))
            if name == "node" and int(completed.stdout.strip().lstrip("v").split(".")[0]) < 22:
                raise RuntimeError("Node.js 22 or newer is required")
        dmg = args.dmg.resolve()
        report["dmg"] = str(dmg)
        report["sha256"] = validate_digest(dmg, args.expected_sha256)
        command(["hdiutil", "verify", str(dmg)], results / "dmg-verify.txt", required=True, timeout=120)
        with tempfile.TemporaryDirectory(prefix="thorium-macos-smoke-") as temp:
            work = Path(temp)
            mount = work / "mount"
            mount.mkdir()
            app = work / "Thorium.app"
            try:
                command(["hdiutil", "attach", "-readonly", "-nobrowse", "-mountpoint", str(mount), str(dmg)],
                        results / "dmg-attach.txt", required=True, timeout=120)
                source = mount / "Thorium.app"
                if not source.is_dir() or source.is_symlink():
                    raise RuntimeError("DMG must contain one regular Thorium.app at its root")
                command(["ditto", str(source), str(app)], results / "copy.txt", required=True, timeout=120)
            finally:
                # Also attempt detachment after an attach/copy failure.
                detached = command(["hdiutil", "detach", str(mount)], results / "dmg-detach.txt")
                if not detached or detached.returncode:
                    command(["hdiutil", "detach", "-force", str(mount)], results / "dmg-detach-force.txt")
            with (app / "Contents/Info.plist").open("rb") as stream:
                info = plistlib.load(stream)
            report["bundle"] = {key: info.get(key) for key in (
                "CFBundleIdentifier", "CFBundleShortVersionString", "CFBundleVersion", "LSMinimumSystemVersion")}
            if info.get("CFBundleShortVersionString") != args.expected_version:
                raise RuntimeError(f"Bundle version mismatch: {report['bundle']}")
            browser = app / "Contents/MacOS/Thorium"
            if info.get("CFBundleExecutable") != "Thorium" or not browser.is_file():
                raise RuntimeError("Missing Thorium bundle executable")
            validate_architectures(app, args.arch, results / "architectures.json")
            for index, binary in enumerate(macho_files(app)):
                if "/MacOS/" in str(binary) or binary.name.endswith("Framework"):
                    command(["xcrun", "dwarfdump", "--uuid", str(binary)], results / f"uuid-{index}.txt")
                    command(["otool", "-l", str(binary)], results / f"load-commands-{index}.txt")
            command(["codesign", "-d", "--verbose=4", "--entitlements", ":-", str(app)], results / "signature-details.txt")
            command(["xattr", "-lr", str(app)], results / "xattrs.txt")
            # Preserve the package's original signature and quarantine state.
            command(["codesign", "--verify", "--deep", "--strict", "--verbose=2", str(app)],
                    results / "signature-verify.txt", required=True)
            run_browser_probes(browser, work, results, args.expected_version, report)
    except Exception as error:
        report["status"] = "FAIL"
        report["errors"].append(str(error))
    finally:
        report["success"] = report["status"] == "PASS"
        write_json(results / "summary.json", report)
        lines = [f"### macOS {report.get('os_version', 'unknown')} / {args.arch}: {report['status']}",
                 f"Expected version: `{args.expected_version}`", "",
                 "| Probe | Overall | Core | Local input/navigation | Startup #1286 | Baidu #1316 | WebGL (diagnostic) | Audio (diagnostic) |",
                 "| --- | --- | --- | --- | --- | --- | --- | --- |"]
        for name, probe in report["probes"].items():
            issues = probe.get("issues", {})
            capabilities = probe.get("capabilities", {})
            issue_statuses = [issues.get(key, {}).get("status", "—") for key in ("1286", "1316")]
            capability_statuses = [capabilities.get(key, {}).get("status", "—") for key in ("webgl", "audio")]
            values = [name, probe.get("status", "INCOMPLETE"), probe.get("core_status", "INCOMPLETE"),
                      (probe.get("interaction") or {}).get("status", "—"),
                      *issue_statuses, *capability_statuses]
            lines.append("| " + " | ".join(values) + " |")
        lines.append("")
        for issue, result in report["probes"].get("gui", {}).get("issues", {}).items():
            reason = " ".join(result.get("reason", "").split())[:1000]
            lines.append(f"- #{issue}: {result.get('status', 'INCOMPLETE')} — {reason}")
        lines += ["", "PASS = completed successfully; FAIL = a check failed; INCOMPLETE = not verified. "
                  "Missing GUI issue results are unverified, not a pass. Startup checks browser/window/renderer "
                  "responsiveness, not native control input.",
                  "", "WebGL and audio are independent diagnostics and do not determine the overall status. "
                  "Crashes, required CDP timeouts and unresponsive renderers during these checks still fail the run. "
                  "See gui/gpu-info.json and browser logs when investigating GPU capability failures.",
                  "", "Baidu CAPTCHA/network blocks remain INCOMPLETE and may coexist with an overall PASS "
                  "only after local HTTP input/navigation and renderer liveness pass. "
                  "Missing inputs/results, crashes and hangs are not exempt. A smoke PASS does not mean #1316 passed. "
                  "The gui-repeat diagnostic uses unchanged GUI arguments with a fresh profile before "
                  "the flag diagnostics. Its result can reveal startup/order effects, but does not identify "
                  "their cause. Diagnostic passes do not replace baseline results. "
                  "See the uploaded JSON, logs and dumps."]
        lines += [f"\nError: {error}" for error in report["errors"]]
        summary = "\n".join(lines) + "\n"
        (results / "summary.md").write_text(summary)
        if os.environ.get("GITHUB_STEP_SUMMARY"):
            with open(os.environ["GITHUB_STEP_SUMMARY"], "a") as stream:
                stream.write(summary)
        print(summary)
    return {"PASS": 0, "FAIL": 1, "INCOMPLETE": 2}[report["status"]]


if __name__ == "__main__":
    raise SystemExit(main())
