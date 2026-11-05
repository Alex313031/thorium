#!/usr/bin/env python3
"""Hand off a macOS application bundle and package the release-named DMG."""

import os
from pathlib import Path
import plistlib
import shutil
import subprocess
import sys
import time

from common import (ROOT, configuration, file_record, input_identity, profile_name,
                    read_json, run, sha256, summary, validate_manifest, write_json)
from macos_host import require_runner

sys.path.insert(0, str(ROOT / "infra/macos"))
from package_dmg import extract_archive, validate_architectures

INPUT_NAMES = ("Thorium.app.tar.gz", "pkg-dmg")


def require_complete(manifest, profile):
    if (manifest["status"] != "complete" or manifest["targets"] != profile["targets"] or
            manifest["phase_index"] != len(profile["targets"])):
        raise ValueError("both macOS build targets must complete before packaging")


def bundle_info(app, version):
    if app.is_symlink() or not app.is_dir():
        raise ValueError("Thorium.app is missing or is a symlink")
    with (app / "Contents/Info.plist").open("rb") as stream:
        info = plistlib.load(stream)
    if (info.get("CFBundleExecutable") != "Thorium" or
            info.get("CFBundleIdentifier") != "org.chromium.Thorium" or
            info.get("CFBundleShortVersionString") != version):
        raise ValueError("application identity or version does not match the source lock")
    binary = app / "Contents/MacOS/Thorium"
    if not binary.is_file() or not os.access(binary, os.X_OK):
        raise ValueError("the application executable is missing or has lost its executable mode")
    return {key: info[key] for key in ("CFBundleExecutable", "CFBundleIdentifier",
                                      "CFBundleShortVersionString", "CFBundleVersion")}


def export_inputs(work, destination):
    require_runner()
    sources, profile = configuration()
    manifest = validate_manifest(work)
    require_complete(manifest, profile)
    out = work / "src" / profile["out_dir"]
    info = bundle_info(out / "Thorium.app", sources["chromium_version"])
    pkg_dmg = work / "src/chrome/installer/mac/pkg-dmg"
    if sha256(pkg_dmg) != sources["macos"]["pkg_dmg_sha256"]:
        raise ValueError("pkg-dmg does not match the pinned Chromium revision")
    destination.mkdir(parents=True, exist_ok=False)
    # Preserve executable modes and the framework's relative symlinks across
    # Actions artifacts; uploading a loose .app would discard those modes.
    run(["gtar", "--format=posix", "-czf", destination / INPUT_NAMES[0],
         "-C", out, "Thorium.app"])
    shutil.copy2(pkg_dmg, destination / INPUT_NAMES[1])
    manifest.update({"files": [file_record(destination / name) for name in INPUT_NAMES],
                     "bundle": info, "macos_host": read_json(work / "macos-host.json")})
    write_json(destination / "manifest.json", manifest)
    if (work / "logs").is_dir():
        shutil.copytree(work / "logs", destination / "logs")


def package(directory, destination, tools_dir):
    require_runner()
    sources, profile = configuration()
    manifest = read_json(directory / "manifest.json")
    require_complete(manifest, profile)
    if manifest["identity"] != input_identity():
        raise ValueError("macOS package inputs do not match this checkout/configuration")
    if manifest["files"] != [file_record(directory / name) for name in INPUT_NAMES]:
        raise ValueError("macOS package inputs are corrupt or unexpected")
    if sha256(directory / "pkg-dmg") != sources["macos"]["pkg_dmg_sha256"]:
        raise ValueError("pkg-dmg does not match the source lock")
    destination.mkdir(parents=True, exist_ok=False)
    tools_dir.mkdir(parents=True, exist_ok=False)
    src = tools_dir / "src"
    out = src / profile["out_dir"]
    extract_archive(directory / "Thorium.app.tar.gz", out)
    info = bundle_info(out / "Thorium.app", sources["chromium_version"])
    if info != manifest["bundle"]:
        raise ValueError("application metadata changed during handoff")
    # Inspect packaged binaries only; do not launch the browser or any tests.
    validate_architectures(out / "Thorium.app", profile["target_cpu"],
                           destination / "macho-architectures.json")
    pkg_dmg = src / "chrome/installer/mac/pkg-dmg"
    pkg_dmg.parent.mkdir(parents=True)
    shutil.copyfile(directory / "pkg-dmg", pkg_dmg)
    pkg_dmg.chmod(0o755)
    image = out / "Thorium_MacOS.dmg"
    for attempt in range(3):
        image.unlink(missing_ok=True)
        try:
            run([sys.executable, ROOT / "create_dmg.py", "--chromium-src", src,
                 "--thorium-root", ROOT])
            break
        except subprocess.CalledProcessError:
            if attempt == 2:
                raise
            time.sleep(10)
    name, = profile["products"]
    file_record(image)
    shutil.copy2(image, destination / name)
    records = [file_record(destination / name)]
    manifest.update({"files": records, "runtime_tests": "not_run", "release_published": False,
                     "ci_code_signing": "ad_hoc", "notarized": False})
    write_json(destination / "manifest.json", manifest)
    (destination / "SHA256SUMS").write_text(
        "".join(f"{item['sha256']}  {item['name']}\n" for item in records))
    if (directory / "logs").is_dir():
        shutil.copytree(directory / "logs", destination / "logs")
    (destination / "README.txt").write_text(
        f"{profile_name()}, Chromium {sources['chromium_version']}\n"
        "Both default build targets and DMG packaging are complete.\n"
        "The application is ad-hoc signed, without Developer ID signing or notarization.\n"
        "Product tests were not run; check and publish manually.\n"
        "Verify SHA256SUMS and manifest.json. Artifacts are not permanent release storage.\n")
    summary(f"Generated `{name}`, SHA256SUMS and manifest. Product tests were not run; "
            "check and publish manually. No Release was published.")
