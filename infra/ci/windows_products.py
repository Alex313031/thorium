#!/usr/bin/env python3
"""Collect Windows installers and create the matching portable ZIPs."""

from pathlib import Path
import shutil
import sys
import tempfile

from common import (ROOT, configuration, file_record, input_identity, profile_name,
                    read_json, summary, validate_manifest, write_json)

sys.path.insert(0, str(ROOT / "infra/portable"))
import portable


def require_complete(manifest, profile):
    if (manifest["status"] != "complete" or manifest["targets"] != profile["targets"] or
            manifest["phase_index"] != len(profile["targets"])):
        raise ValueError("both Windows build targets must complete before packaging")


def export_inputs(work, destination):
    sources, profile = configuration()
    manifest = validate_manifest(work)
    require_complete(manifest, profile)
    out = work / "src" / profile["out_dir"]
    name = profile["products"][0].format(version=sources["chromium_version"])
    installer = out / name
    file_record(installer)
    # The installer bootstrap may have a different architecture from its payload.
    installer_cpu = portable.windows_executable_architecture(installer)
    if portable.windows_executable_architecture(out / "thorium.exe") != profile["target_cpu"]:
        raise ValueError("browser executable does not match the requested Windows architecture")
    destination.mkdir(parents=True, exist_ok=False)
    shutil.copy2(installer, destination / name)
    manifest.update({"files": [file_record(destination / name)], "installer_cpu": installer_cpu,
                     "windows_host": read_json(work / "windows-host.json")})
    write_json(destination / "manifest.json", manifest)
    if (work / "logs").is_dir():
        shutil.copytree(work / "logs", destination / "logs")


def package(directory, destination):
    sources, profile = configuration()
    manifest = read_json(directory / "manifest.json")
    require_complete(manifest, profile)
    if manifest["identity"] != input_identity():
        raise ValueError("Windows package input identity does not match this checkout/configuration")
    version = sources["chromium_version"]
    installer_name, zip_name = [name.format(version=version) for name in profile["products"]]
    installer = directory / installer_name
    if manifest["files"] != [file_record(installer)]:
        raise ValueError("Windows installer input is corrupt or unexpected")
    destination.mkdir(parents=True, exist_ok=False)
    # Reuse the portable packager's extraction, version/PE inspection and
    # launchers. Inspecting the archive never executes the installer or browser.
    with tempfile.TemporaryDirectory(prefix="thorium-portable-") as temporary:
        work = Path(temporary)
        staging = work / "staging"
        metadata, variant = portable.prepare_windows_package(
            installer, staging, work, None, profile["portable_profile"], version)
        if metadata.architecture != profile["target_cpu"]:
            raise ValueError("installer payload does not match the requested Windows architecture")
        expected = portable.default_output(installer, "windows", version, variant).name
        if expected != zip_name:
            raise ValueError(f"unexpected portable filename: {expected}; expected {zip_name}")
        portable.copy_support_files(ROOT / "infra/portable", staging, metadata)
        portable.write_zip(staging, destination / zip_name, force=False)
    shutil.copy2(installer, destination / installer_name)
    records = [file_record(destination / name) for name in (installer_name, zip_name)]
    manifest.update({"files": records, "runtime_tests": "not_run", "release_published": False,
                     "ci_code_signing": "not_configured"})
    write_json(destination / "manifest.json", manifest)
    (destination / "SHA256SUMS").write_text(
        "".join(f"{item['sha256']}  {item['name']}\n" for item in records))
    if (directory / "logs").is_dir():
        shutil.copytree(directory / "logs", destination / "logs")
    (destination / "README.txt").write_text(
        f"{profile_name()}, Chromium {version}\n"
        "Both default build targets and the installer/portable ZIP are complete.\n"
        "Product tests were not run; check and publish manually.\n"
        "Verify SHA256SUMS and manifest.json. Artifacts are not permanent release storage.\n")
    summary(f"Both {profile_name()} packages, checksums and manifest have been generated. "
            "Product tests were not run; no Release was published.\n\n" +
            "\n".join(f"- `{item['name']}` ({item['size_bytes']} bytes)" for item in records))
