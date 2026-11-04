#!/usr/bin/env python3
"""Export the three release APKs without installing or running Android products."""

from pathlib import Path
import re
import shutil
import struct
import subprocess
import zipfile

from common import (configuration, file_record, input_identity, profile_name, read_json,
                    summary, validate_manifest, write_json)


def apk_abis(path, expected):
    machines = {"armeabi-v7a": (1, 40), "arm64-v8a": (2, 183)}
    abis = set()
    with zipfile.ZipFile(path) as package:
        members = package.namelist()
        if "AndroidManifest.xml" not in members or "classes.dex" not in members:
            raise ValueError(f"incomplete Android package: {path}")
        for member in members:
            parts = Path(member).parts
            if len(parts) != 3 or parts[0] != "lib" or not member.endswith(".so"):
                continue
            abi = parts[1]
            with package.open(member) as stream:
                header = stream.read(20)
            if (abi not in machines or len(header) < 20 or header[:4] != b"\x7fELF" or
                    header[5] != 1 or
                    (header[4], struct.unpack_from("<H", header, 18)[0]) != machines[abi]):
                raise ValueError(f"unexpected native library architecture: {path.name}: {member}")
            abis.add(abi)
    if expected not in abis or (expected == "armeabi-v7a" and abis != {expected}):
        raise ValueError(f"APK does not match the requested ABI: {path.name}: {sorted(abis)}")
    return sorted(abis)


def require_complete(manifest, profile):
    if (manifest["status"] != "complete" or manifest["targets"] != profile["targets"] or
            manifest["phase_index"] != len(profile["targets"])):
        raise ValueError("all three Android build targets must complete before packaging")


def export_inputs(work, destination):
    sources, profile = configuration()
    manifest = validate_manifest(work)
    require_complete(manifest, profile)
    apk_dir = work / "src" / profile["out_dir"] / "apks"
    aapt2 = work / "src/third_party/android_build_tools/aapt2/cipd/aapt2"
    metadata = {}
    # Read APK metadata with the versioned SDK tool; no emulator or device is used.
    for name in profile["products"]:
        path = apk_dir / name
        file_record(path)
        abis = apk_abis(path, profile["apk_abi"])
        badging = subprocess.check_output([str(aapt2), "dump", "badging", str(path)], text=True)
        package = next((line for line in badging.splitlines() if line.startswith("package: ")), "")
        fields = dict(re.findall(r"(\w+)='([^']*)'", package))
        if not all(fields.get(key) for key in ("name", "versionCode", "versionName")):
            raise ValueError(f"APK metadata is incomplete: {name}")
        # Content Shell owns its manifest version; browser/WebView use Chromium's version.
        if not name.startswith("Thorium_Shell_") and fields["versionName"] != sources["chromium_version"]:
            raise ValueError(f"APK version does not match the source lock: {name}: {fields['versionName']}")
        metadata[name] = {"package": fields["name"], "version_code": fields["versionCode"],
                          "version_name": fields["versionName"], "native_abis": abis}
    destination.mkdir(parents=True, exist_ok=False)
    for name in profile["products"]:
        shutil.copy2(apk_dir / name, destination / name)
    manifest.update({"files": [file_record(destination / name) for name in profile["products"]],
                     "apk_metadata": metadata, "apk_signing": "GN-configured; no CI re-signing"})
    write_json(destination / "manifest.json", manifest)
    if (work / "logs").is_dir():
        shutil.copytree(work / "logs", destination / "logs")


def package(directory, destination):
    sources, profile = configuration()
    manifest = read_json(directory / "manifest.json")
    require_complete(manifest, profile)
    if manifest["identity"] != input_identity():
        raise ValueError("APK input identity does not match this checkout/configuration")
    if [item["name"] for item in manifest["files"]] != profile["products"]:
        raise ValueError("unexpected APK artifact list")
    for item in manifest["files"]:
        path = directory / item["name"]
        if file_record(path) != item:
            raise ValueError(f"APK input is corrupt: {item['name']}")
        if apk_abis(path, profile["apk_abi"]) != manifest["apk_metadata"][item["name"]]["native_abis"]:
            raise ValueError(f"APK ABI metadata changed: {item['name']}")
    destination.mkdir(parents=True, exist_ok=False)
    for item in manifest["files"]:
        shutil.copy2(directory / item["name"], destination / item["name"])
    manifest.update({"runtime_tests": "not_run", "release_published": False})
    write_json(destination / "manifest.json", manifest)
    (destination / "SHA256SUMS").write_text(
        "".join(f"{item['sha256']}  {item['name']}\n" for item in manifest["files"]))
    if (directory / "logs").is_dir():
        shutil.copytree(directory / "logs", destination / "logs")
    (destination / "README.txt").write_text(
        f"{profile_name()}, Chromium {sources['chromium_version']}\n"
        "Browser, Content Shell and System WebView APKs retain their build filenames and signatures.\n"
        "Product tests were not run; check and publish manually.\n"
        "Verify SHA256SUMS and manifest.json. Artifacts are not permanent release storage.\n")
    summary(f"All three {profile_name()} APKs, checksums and manifest have been generated. "
            "Product tests were not run; no Release was published.\n\n" +
            "\n".join(f"- `{item['name']}` ({item['size_bytes']} bytes)" for item in manifest["files"]))
