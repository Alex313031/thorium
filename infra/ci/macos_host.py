#!/usr/bin/env python3
"""Provision native macOS runners and identify the Xcode environment."""

import argparse
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys
import tempfile

from common import configuration, read_json, run, sha256, summary, write_json

VOLUME = Path("/Volumes/ThoriumCI")
WORK_ROOT = VOLUME / "work"


def require_runner():
    _, profile = configuration()
    cpu = {"x86_64": "x64", "arm64": "arm64"}.get(platform.machine())
    if (sys.platform != "darwin" or profile["target_os"] != "mac" or
            cpu != profile["host_cpu"] or cpu != profile["target_cpu"] or
            os.environ.get("GITHUB_ACTIONS") != "true" or
            os.environ.get("RUNNER_ENVIRONMENT") != "github-hosted"):
        raise ValueError("a matching native GitHub-hosted macOS runner is required")
    translated = subprocess.run(["sysctl", "-in", "sysctl.proc_translated"],
                                capture_output=True, text=True)
    if translated.stdout.strip() == "1":
        raise ValueError("run the build with native Python, not under Rosetta")
    if sys.version_info < (3, 11, 8):
        raise ValueError("archive preparation requires Python 3.11.8 or newer")
    return cpu


def inspect():
    cpu = require_runner()
    sources, _ = configuration()
    spec = sources["macos"]
    developer = Path(f"/Applications/Xcode_{spec['xcode_version']}.app/Contents/Developer")
    env = dict(os.environ, DEVELOPER_DIR=str(developer))
    def capture(*command):
        return subprocess.check_output(command, env=env, text=True).strip()
    xcode = capture("xcodebuild", "-version")
    sdk_version = capture("xcrun", "--sdk", "macosx", "--show-sdk-version")
    sdk_build = capture("xcrun", "--sdk", "macosx", "--show-sdk-build-version")
    sdk = Path(capture("xcrun", "--sdk", "macosx", "--show-sdk-path"))
    if (xcode.splitlines() != [f"Xcode {spec['xcode_version']}", f"Build version {spec['xcode_build']}"] or
            sdk_version != spec["sdk_version"] or sdk_build != spec["sdk_build"]):
        raise ValueError(f"unexpected Xcode/SDK: {xcode}; macOS SDK {sdk_version} ({sdk_build})")
    anchors = [developer / "usr/bin/xcodebuild", sdk / "SDKSettings.json",
               sdk / "System/Library/CoreServices/SystemVersion.plist"]
    return {"host_cpu": cpu, "developer_dir": str(developer), "xcode": xcode,
            "sdk_path": str(sdk), "sdk_version": sdk_version, "sdk_build": sdk_build,
            "python_version": platform.python_version(), "python_path": sys.executable,
            "files": {str(path): sha256(path) for path in anchors}}


def bootstrap(report_path, packaging_only):
    require_runner()
    sources, _ = configuration()
    developer = Path(f"/Applications/Xcode_{sources['macos']['xcode_version']}.app/Contents/Developer")
    if not developer.is_dir():
        raise ValueError(f"required Xcode is not installed: {developer}")
    run(["sudo", "xcode-select", "--switch", developer])
    os.environ["DEVELOPER_DIR"] = str(developer)
    with open(os.environ["GITHUB_ENV"], "a") as stream:
        stream.write(f"DEVELOPER_DIR={developer}\n")
    for tool in ("gtar", "zstd", "git", "hdiutil", "codesign", "xattr", "perl"):
        if not shutil.which(tool):
            raise ValueError(f"runner is missing required tool: {tool}")
    if "GNU tar" not in subprocess.check_output(["gtar", "--version"], text=True):
        raise ValueError("GNU tar is required to preserve checkpoint timestamps")
    host = inspect()
    if not packaging_only:
        # These paths belong to the disposable runner, never the source export.
        keep = developer.parent.parent.resolve()
        for app in Path("/Applications").glob("Xcode_*.app"):
            if app.resolve() != keep:
                run(["sudo", "rm", "-rf", "--", app])
        for pattern in ("Google Chrome*.app", "Microsoft Edge*.app", "Firefox*.app"):
            for app in Path("/Applications").glob(pattern):
                run(["sudo", "rm", "-rf", "--", app])
        for relative in ("Library/Developer/CoreSimulator/Caches", "Library/Developer/Xcode/DerivedData",
                         "Library/Caches/Homebrew"):
            path = Path.home() / relative
            if path.exists():
                run(["sudo", "rm", "-rf", "--", path])
        free = shutil.disk_usage(os.environ["RUNNER_TEMP"]).free
        print(f"Physical free space before creating the build volume: {free / 1024 ** 3:.1f} GiB", flush=True)
        if free < 60 * 1024 ** 3:
            raise ValueError(f"insufficient free space before source preparation/restore: {free / 1024 ** 3:.1f} GiB")
        # A sparse APFS image preserves case-distinct upstream paths while using
        # only the space actually written. Its nominal size is not free disk.
        image = Path(os.environ["RUNNER_TEMP"]) / "thorium-build.sparseimage"
        if image.exists() or VOLUME.exists():
            raise ValueError("a fresh macOS build volume is required")
        # hdiutil requires the filesystem personality name, not the APFSX alias.
        run(["hdiutil", "create", "-size", "300g", "-type", "SPARSE", "-fs", "Case-sensitive APFS",
             "-volname", "ThoriumCI", image])
        run(["hdiutil", "attach", "-nobrowse", "-owners", "off", "-mountpoint", VOLUME, image])
        with tempfile.TemporaryDirectory(prefix=".case-check-", dir=VOLUME) as temporary:
            probe = Path(temporary) / "CaseSensitive"
            probe.touch()
            if probe.with_name("casesensitive").exists():
                raise ValueError("the build volume must be case-sensitive")
    report = {**host, "runner_image": os.environ.get("ImageVersion", "unknown"),
              "backing_free_bytes": shutil.disk_usage(os.environ["RUNNER_TEMP"]).free,
              "cpus": os.cpu_count(),
              "memory_bytes": int(subprocess.check_output(["sysctl", "-n", "hw.memsize"], text=True))}
    write_json(report_path, report)
    summary(f"Native macOS {host['host_cpu']}: {host['xcode'].replace(chr(10), ', ')}, "
            f"SDK {host['sdk_version']} ({host['sdk_build']}); "
            f"physical free space {report['backing_free_bytes'] / 1024 ** 3:.1f} GiB.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("bootstrap", "verify"))
    parser.add_argument("--report", type=Path)
    parser.add_argument("--work-root", type=Path)
    parser.add_argument("--packaging-only", action="store_true")
    args = parser.parse_args()
    if args.command == "bootstrap":
        if args.report is None:
            parser.error("bootstrap requires --report")
        bootstrap(args.report, args.packaging_only)
    else:
        if args.work_root is None:
            parser.error("verify requires --work-root")
        if inspect() != read_json(args.work_root / "macos-host.json"):
            raise ValueError("macOS Xcode/SDK/Python changed since preparation; start a fresh build")
        summary("The native macOS environment matches the checkpoint.")


if __name__ == "__main__":
    main()
