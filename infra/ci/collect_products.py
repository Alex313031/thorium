#!/usr/bin/env python3
"""Collect the profile's release packages using the existing packaging tools."""

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys

from common import (ROOT, configuration, download, file_record, input_identity, profile_name,
                    read_json, run, summary, validate_manifest, write_json)


def check_packages(directory, version, profile):
    deb = directory / profile["products"][0].format(version=version)
    rpm = deb.with_suffix(".rpm")
    for path in (deb, rpm):
        file_record(path)
    deb_arch = subprocess.check_output(["dpkg-deb", "-f", str(deb), "Architecture"], text=True).strip()
    deb_version = subprocess.check_output(["dpkg-deb", "-f", str(deb), "Version"], text=True).strip()
    rpm_info = subprocess.check_output(["rpm", "-qp", "--qf", "%{VERSION}\n%{ARCH}\n",
                                        str(rpm)], text=True).splitlines()
    if deb_arch != profile["deb_arch"] or deb_version.rsplit("-", 1)[0] != version:
        raise ValueError(f"unexpected DEB metadata: {deb_arch} {deb_version}")
    if rpm_info != [version, profile["rpm_arch"]]:
        raise ValueError(f"unexpected RPM metadata: {rpm_info}")
    return deb, rpm


def export_inputs(work, destination):
    sources, profile = configuration()
    manifest = validate_manifest(work)
    if manifest["status"] != "complete" or manifest["phase_index"] != len(profile["targets"]):
        raise ValueError("all default phases must complete before exporting package inputs")
    if manifest["targets"] != profile["targets"]:
        raise ValueError("incomplete or unexpected build target list")
    deb, rpm = check_packages(work / "src" / profile["out_dir"], sources["chromium_version"], profile)
    destination.mkdir(parents=True, exist_ok=False)
    for path in (deb, rpm):
        shutil.copy2(path, destination / path.name)
    manifest["files"] = [file_record(destination / path.name) for path in (deb, rpm)]
    write_json(destination / "manifest.json", manifest)
    if (work / "logs").is_dir():
        shutil.copytree(work / "logs", destination / "logs")


def verify_inputs(directory):
    manifest = read_json(directory / "manifest.json")
    sources, profile = configuration()
    if manifest["identity"] != input_identity() or manifest["status"] != "complete":
        raise ValueError("package input identity or completion status does not match")
    if manifest["targets"] != profile["targets"] or manifest["phase_index"] != len(profile["targets"]):
        raise ValueError("package input does not record all completed build phases")
    expected = [name.format(version=sources["chromium_version"]) for name in profile["products"][:2]]
    if [f["name"] for f in manifest["files"]] != expected:
        raise ValueError("wrong package input list")
    for item in manifest["files"]:
        if file_record(directory / item["name"]) != item:
            raise ValueError(f"package input is corrupt: {item['name']}")
    check_packages(directory, sources["chromium_version"], profile)
    return manifest


def package(directory, destination, tools_dir):
    sources, profile = configuration()
    manifest = verify_inputs(directory)
    destination.mkdir(parents=True, exist_ok=False)
    env = dict(os.environ)
    version = sources["chromium_version"]
    names = [name.format(version=version) for name in profile["products"]]
    appimage_name = next((name for name in names if name.endswith(".AppImage")), None)
    deb = directory / names[0]
    # Linux i386 is identified by DEB architecture, not a portable SIMD profile.
    portable_args = ["--profile", profile["portable_profile"]] if profile["portable_profile"] else []
    run([sys.executable, ROOT / "infra/portable/portable.py", "--platform", "linux",
         "--input", deb, *portable_args, "--expected-version", version,
         "--output", destination / names[2]], env=env)
    if appimage_name:
        tools_dir.mkdir(parents=True, exist_ok=False)
        for item in sources["appimage"]["dependencies"]:
            downloaded = download(item, tools_dir / "cache")
            path = tools_dir / item["path"]
            shutil.copyfile(downloaded, path)
            if item.get("executable"):
                path.chmod(0o755)
        env.update({"PATH": f"{tools_dir}:{env['PATH']}",
                    "FUNCTIONS_SH": str(tools_dir / "appimage-functions.sh"),
                    "STATIC_FILES": str(tools_dir),
                    "APPIMAGE_RUNTIME_FILE": str(tools_dir / profile["appimage_runtime"]),
                    "PKG2AICOMMIT": sources["appimage"]["pkg2appimage_commit"],
                    "DOCKER_BUILD": "1", "ARCH": profile["appimage_arch"]})
        # Extract the packaging tool without FUSE or executing the Thorium payload.
        run([sys.executable, ROOT / "infra/APPIMAGE/appimage.py", "build", deb], env=env)
        shutil.copy2(ROOT / "infra/APPIMAGE/out" / appimage_name, destination / appimage_name)
    for name in names[:2]:
        shutil.copy2(directory / name, destination / name)
    records = [file_record(destination / name) for name in names]
    manifest.update({"files": records, "runtime_tests": "not_run",
                     "appimage_tools": sources["appimage"] if appimage_name else None,
                     "release_published": False})
    write_json(destination / "manifest.json", manifest)
    (destination / "SHA256SUMS").write_text("".join(f"{item['sha256']}  {item['name']}\n" for item in records))
    if (directory / "logs").is_dir():
        shutil.copytree(directory / "logs", destination / "logs")
    readme = (
        profile_name() + ", Chromium " + version + "\n"
        f"All three default build phases and {len(records)} package formats are complete. Product tests were not run; check and publish manually.\n"
        "Verify SHA256SUMS and manifest.json. Artifacts are not permanent release storage.\n")
    if appimage_name:
        readme += "After downloading the AppImage directly from Actions, use chmod +x to restore its executable permission if needed.\n"
    (destination / "README.txt").write_text(readme)
    summary(f"Build stages: {manifest['stage']}; resumptions between build stages: {manifest['resumed_stages']}.\n\n"
            f"All {len(records)} {profile_name()} packages, checksums and manifest have been generated. Product tests were not run; no Release was published.\n\n" +
            "\n".join(f"- `{item['name']}` ({item['size_bytes']} bytes)" for item in records))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    export = sub.add_parser("export")
    export.add_argument("--work-root", type=Path, required=True)
    export.add_argument("--destination", type=Path, required=True)
    pack = sub.add_parser("package")
    pack.add_argument("--inputs", type=Path, required=True)
    pack.add_argument("--destination", type=Path, required=True)
    pack.add_argument("--tools", type=Path, required=True)
    args = parser.parse_args()
    _, profile = configuration()
    if profile["target_os"] == "mac":
        from macos_products import export_inputs as export_macos, package as package_macos
        if args.command == "export":
            export_macos(args.work_root.resolve(), args.destination.resolve())
        else:
            package_macos(args.inputs.resolve(), args.destination.resolve(), args.tools.resolve())
        return
    if profile["target_os"] == "win":
        from windows_products import export_inputs as export_windows, package as package_windows
        if args.command == "export":
            export_windows(args.work_root.resolve(), args.destination.resolve())
        else:
            package_windows(args.inputs.resolve(), args.destination.resolve())
        return
    if profile["target_os"] == "android":
        from android_products import export_inputs as export_android, package as package_android
        if args.command == "export":
            export_android(args.work_root.resolve(), args.destination.resolve())
        else:
            package_android(args.inputs.resolve(), args.destination.resolve())
        return
    if args.command == "export":
        export_inputs(args.work_root.resolve(), args.destination.resolve())
    else:
        package(args.inputs.resolve(), args.destination.resolve(), args.tools.resolve())


if __name__ == "__main__":
    main()
