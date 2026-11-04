#!/usr/bin/env python3
"""Provision and identify the native Windows build environment."""

import argparse
import json
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys

from common import CONFIG, download, read_json, sha256, summary, write_json


def require_runner():
    if (os.name != "nt" or platform.machine().lower() not in ("amd64", "x86_64") or
            os.environ.get("GITHUB_ACTIONS") != "true" or
            os.environ.get("RUNNER_ENVIRONMENT") != "github-hosted"):
        raise ValueError("native Windows builds require a GitHub-hosted Windows x64 runner")
    if sys.version_info < (3, 11, 8):
        raise ValueError("Windows archive preparation requires Python 3.11.8 or newer")


def sdk_files(sdk, version):
    required = [sdk / f"Include/{version}/um/Windows.h", sdk / f"Include/{version}/shared/sdkddkver.h",
                sdk / f"bin/{version}/x64/rc.exe", sdk / "Debuggers/x64/dbghelp.dll"]
    for arch in ("x86", "x64", "arm64"):
        required.extend([sdk / f"Lib/{version}/um/{arch}/kernel32.lib",
                         sdk / f"Lib/{version}/ucrt/{arch}/ucrt.lib"])
    return required


def inspect():
    require_runner()
    spec = read_json(CONFIG / "windows-sources.json")
    program_files = Path(os.environ["ProgramFiles(x86)"])
    vswhere = program_files / "Microsoft Visual Studio/Installer/vswhere.exe"
    major = spec["visual_studio_minimum_major"]
    installations = json.loads(subprocess.check_output([
        str(vswhere), "-latest", "-products", "*", "-version", f"[{major}.0,{major + 1}.0)",
        "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
        "Microsoft.VisualStudio.Component.VC.ATLMFC", "Microsoft.VisualStudio.Component.VC.Tools.ARM64",
        "Microsoft.VisualStudio.Component.VC.MFC.ARM64", "-format", "json", "-utf8"], encoding="utf-8"))
    if len(installations) != 1:
        raise ValueError(f"VS {spec['visual_studio_year']} with x64/x86 and ARM64 C++, ATL/MFC components is required")
    installation = installations[0]
    vs = Path(installation["installationPath"])
    sdk = program_files / "Windows Kits/10"
    version = spec["sdk_installer"]["directory_version"]
    required = sdk_files(sdk, version)
    toolset_file = vs / "VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt"
    toolset = toolset_file.read_text().strip()
    required.extend([toolset_file, vs / f"VC/Tools/MSVC/{toolset}/include/vcruntime.h"])
    for arch in ("x86", "x64", "arm64"):
        required.extend([vs / f"VC/Tools/MSVC/{toolset}/lib/{arch}/libcmt.lib",
                         vs / f"VC/Tools/MSVC/{toolset}/atlmfc/lib/{arch}/atls.lib"])
    missing = [str(path) for path in required if not path.is_file()]
    if missing:
        raise ValueError("Windows toolchain files are missing: " + ", ".join(missing))
    # Generated Ninja commands contain these absolute paths. Reject a changed
    # toolset on resume instead of mixing outputs from different runner images.
    return {"vs_path": str(vs), "vs_version": installation["installationVersion"],
            "vs_year": spec["visual_studio_year"],
            "vc_tools_version": toolset, "sdk_path": str(sdk), "sdk_directory_version": version,
            "python_version": platform.python_version(), "python_path": sys.executable,
            "files": {str(path): sha256(path) for path in required}}


def bootstrap(cache):
    require_runner()
    spec = read_json(CONFIG / "windows-sources.json")["sdk_installer"]
    # SDK servicing releases share a directory name. Always invoke the locked
    # installer instead of treating that directory's existence as a version check.
    archive = download(spec, cache)
    installer = Path(cache) / "winsdksetup.exe"
    shutil.copyfile(archive, installer)
    code = subprocess.call([str(installer), "/q", "/norestart", "/ceip", "off", "/features",
                            "OptionId.DesktopCPPx86", "OptionId.DesktopCPPx64",
                            "OptionId.DesktopCPParm64", "OptionId.UWPCPP",
                            "OptionId.WindowsDesktopDebuggers"])
    if code not in (0, 3010):
        raise RuntimeError(f"Windows SDK installer failed: {code}")
    report = inspect()
    summary(f"Native toolchain: VS {report['vs_version']}, MSVC {report['vc_tools_version']}, "
            f"Windows SDK {report['sdk_directory_version']}.")
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("bootstrap", "verify"))
    parser.add_argument("--cache", type=Path)
    parser.add_argument("--work-root", type=Path)
    parser.add_argument("--report", type=Path)
    args = parser.parse_args()
    if args.command == "bootstrap":
        report = bootstrap(args.cache)
        report["runner_image"] = os.environ.get("ImageVersion", "unknown")
        report["free_bytes"] = shutil.disk_usage(args.cache).free
        write_json(args.report, report)
    else:
        if inspect() != read_json(args.work_root / "windows-host.json"):
            raise ValueError("Windows toolchain changed since preparation; start a fresh build")
        summary("The native Windows toolchain matches the checkpoint.")


if __name__ == "__main__":
    main()
