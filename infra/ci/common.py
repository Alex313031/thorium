#!/usr/bin/env python3
"""Shared primitives for Thorium Actions builds on Linux hosts."""

import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import stat
import subprocess
import tarfile
import time
import urllib.request
import zipfile

ROOT = Path(__file__).resolve().parents[2]
CONFIG = Path(__file__).resolve().parent
DEFAULT_PROFILE = "linux-x64-avx2"


def read_json(path):
    return json.loads(Path(path).read_text())


def write_json(path, value):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(path.suffix + ".new")
    temporary.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n")
    temporary.replace(path)


def sha256(path):
    with Path(path).open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def file_record(path):
    if not path.is_file() or path.stat().st_size == 0 or path.is_symlink():
        raise ValueError(f"missing, empty or symlinked product: {path}")
    return {"name": path.name, "size_bytes": path.stat().st_size, "sha256": sha256(path)}


def digest_json(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True).encode()).hexdigest()


def profile_name():
    return os.environ.get("THORIUM_CI_PROFILE", DEFAULT_PROFILE)


def profiles():
    return read_json(CONFIG / "profiles.json")


def configuration(selected=None):
    sources = read_json(CONFIG / "sources.json")
    selected = selected or profile_name()
    available = profiles()
    if selected not in available:
        raise ValueError(f"unknown build profile: {selected}")
    profile = available[selected]
    version = re.search(r'^THORIUM_VERSION = "([0-9.]+)"$',
                        (ROOT / "version.py").read_text(), re.M)
    if not version or version[1] != sources["chromium_version"]:
        raise ValueError("version.py and infra/ci/sources.json disagree; update the source lock")
    if profile["target_os"] == "android":
        android = read_json(CONFIG / "android-sources.json")
        if (android["chromium_revision"] != sources["chromium_revision"] or
                android["deps_sha256"] != sources["deps_sha256"]):
            raise ValueError("Android inputs do not match the Chromium source lock")
        sources["dependencies"].extend(android["dependencies"])
    # The host stays x64; only target-specific inputs differ between profiles.
    for owner in (sources, sources["appimage"]):
        owner["dependencies"] = [item for item in owner["dependencies"]
                                 if "target_cpus" not in item or
                                 profile["target_cpu"] in item["target_cpus"]]
    if not any(name.endswith(".AppImage") for name in profile["products"]):
        sources["appimage"]["dependencies"] = []
    return sources, profile


def input_identity():
    sources, profile = configuration()
    # Include actual input bytes so a local verification with uncommitted edits
    # cannot be mistaken for a build of an unchanged commit.
    files = {}
    for relative in ("setup.py", "build.py", "version.py", "args.gn", "arm", "src", "other",
                     "patch_scripts", "pak_src", "thorium_shell", "infra/ci",
                     "infra/portable", "infra/APPIMAGE", "logos",
                     ".github/workflows/build-linux.yml",
                     ".github/workflows/build-android.yml",
                     ".github/workflows/build-profile.yml",
                     ".github/workflows/build-stage.yml"):
        path = ROOT / relative
        candidates = path.rglob("*") if path.is_dir() else [path]
        for item in candidates:
            rel = item.relative_to(ROOT)
            if "__pycache__" in rel.parts or "out" in rel.parts or item.suffix == ".pyc":
                continue
            if item.is_symlink():
                files[str(rel)] = "link:" + os.readlink(item)
            elif item.is_file():
                files[str(rel)] = sha256(item)
    return {
        "schema_version": 2, "profile": profile_name(),
        "thorium_commit": subprocess.check_output(
            ["git", "-C", str(ROOT), "rev-parse", "HEAD"], text=True).strip(),
        "chromium_version": sources["chromium_version"],
        "chromium_revision": sources["chromium_revision"],
        "source_sha256": sources["chromium"]["sha256"],
        "inputs_sha256": digest_json(files),
        "configuration_sha256": digest_json([sources, profile]),
        "backend": "siso", "remote_execution": False, "pruning_mode": "none",
    }


def checked_path(root, relative):
    root = Path(root).resolve()
    path = root / relative
    if Path(relative).is_absolute() or not path.resolve().is_relative_to(root):
        raise ValueError(f"path escapes its root: {relative}")
    return path


def tar_contents_sha256(path):
    """Hash exported files, modes and links, ignoring Gitiles' varying timestamps."""
    records, names = [], set()
    with tarfile.open(path, "r|*") as archive:
        for member in archive:
            if member.name in names or not (member.isfile() or member.isdir() or member.issym()):
                raise ValueError(f"unsupported or duplicate archive member: {member.name}")
            names.add(member.name)
            content = ""
            if member.isfile():
                with archive.extractfile(member) as stream:
                    content = hashlib.file_digest(stream, "sha256").hexdigest()
            records.append([member.name, member.type.decode("ascii"), member.mode & 0o777,
                            member.linkname, content])
    return digest_json(sorted(records))


def download(item, cache):
    """Only accept locked HTTPS inputs; publish the cache entry after verification."""
    url = item["url"]
    content_lock = "tar_contents_sha256" in item
    if content_lock and (item.get("kind") != "tar" or "sha256" in item):
        raise ValueError("tar content locks require a tar input and cannot mix digest schemes")
    expected = item["tar_contents_sha256" if content_lock else "sha256"]
    digest = tar_contents_sha256 if content_lock else sha256
    if not url.startswith("https://") or not re.fullmatch(r"[0-9a-f]{64}", expected):
        raise ValueError("downloads require HTTPS and a locked SHA-256")
    cache = Path(cache)
    cache.mkdir(parents=True, exist_ok=True)
    destination = cache / expected
    if destination.exists():
        try:
            if digest(destination) == expected:
                return destination
        except (OSError, ValueError, tarfile.TarError):
            pass
    temporary = destination.with_suffix(".partial")
    for attempt in range(3):
        try:
            print(f"Downloading {url}", flush=True)
            request = urllib.request.Request(url, headers=item.get("headers", {}))
            with urllib.request.urlopen(request, timeout=120) as response, temporary.open("wb") as out:
                shutil.copyfileobj(response, out, 1024 * 1024)
            if item.get("size_bytes") and temporary.stat().st_size != item["size_bytes"]:
                raise ValueError(f"download size mismatch: {url}")
            if digest(temporary) != expected:
                raise ValueError(f"download SHA-256 mismatch: {url}")
            temporary.replace(destination)
            return destination
        except (OSError, ValueError, tarfile.TarError):
            temporary.unlink(missing_ok=True)
            if attempt == 2:
                raise
            time.sleep(2 ** attempt)


def unpack(archive, destination, kind="tar"):
    """Preserve executable bits and internal links, reject escaping paths/devices."""
    destination = Path(destination).resolve()
    destination.mkdir(parents=True, exist_ok=True)
    if kind == "zip":
        with zipfile.ZipFile(archive) as package:
            for member in package.infolist():
                path = checked_path(destination, member.filename)
                mode = member.external_attr >> 16
                if stat.S_ISLNK(mode):
                    target = package.read(member).decode()
                    if not (path.parent / target).resolve().is_relative_to(destination):
                        raise ValueError(f"escaping archive link: {member.filename}")
                    path.parent.mkdir(parents=True, exist_ok=True)
                    path.unlink(missing_ok=True)
                    path.symlink_to(target)
                elif member.is_dir():
                    path.mkdir(parents=True, exist_ok=True)
                else:
                    path.parent.mkdir(parents=True, exist_ok=True)
                    # CIPD packages can overlay read-only files from the export
                    # or another package installed into the same directory.
                    path.unlink(missing_ok=True)
                    with package.open(member) as src, path.open("wb") as out:
                        shutil.copyfileobj(src, out)
                    path.chmod((mode & 0o777) or 0o644)
        return
    with tarfile.open(archive, "r|*") as package:
        for member in package:
            # Python >=3.11.8 provides the data filter. Do not silently use the
            # old, unrestricted extraction if an unsupported interpreter is used.
            package.extract(member, destination, filter="data")


def run(command, cwd=None, env=None):
    print("+ " + " ".join(map(str, command)), flush=True)
    subprocess.run(list(map(str, command)), cwd=cwd, env=env, check=True)


def build_environment(work):
    work = Path(work).resolve()
    env = dict(os.environ)
    for key in list(env):
        if key.startswith(("GIT_", "RBE_", "RECLIENT_", "SISO_")):
            env.pop(key)
    env.update({
        "PATH": f"{work / 'depot_tools'}:{env['PATH']}",
        "DEPOT_TOOLS_UPDATE": "0", "DEPOT_TOOLS_METRICS": "0",
        "DEPOT_TOOLS_COLLECT_METRICS": "0",
        "VPYTHON_BYPASS": "manually managed python not supported by chrome operations",
        "GIT_CEILING_DIRECTORIES": str(work),
        "XDG_CACHE_HOME": str(work / ".cache"),
        "CR_DIR": str(work / "src"), "THOR_DIR": str(ROOT),
        "PYTHONUNBUFFERED": "1",
    })
    return env


def output(name, value):
    if os.environ.get("GITHUB_OUTPUT"):
        with open(os.environ["GITHUB_OUTPUT"], "a") as stream:
            stream.write(f"{name}={value}\n")


def summary(text):
    print(text, flush=True)
    if os.environ.get("GITHUB_STEP_SUMMARY"):
        with open(os.environ["GITHUB_STEP_SUMMARY"], "a") as stream:
            stream.write(text + "\n")


def validate_manifest(work):
    work = Path(work).resolve()
    manifest = read_json(work / "manifest.json")
    if manifest["identity"] != input_identity():
        raise ValueError("checkpoint inputs do not match this Thorium checkout/configuration")
    if manifest["work_root"] != str(work):
        raise ValueError("checkpoint absolute path changed; refusing an incompatible restore")
    if sha256(work / "src/out/thorium/args.gn") != manifest["args_sha256"]:
        raise ValueError("checkpoint GN args changed")
    sources, _ = configuration()
    if manifest["identity"]["source_sha256"] != sources["chromium"]["sha256"]:
        raise ValueError("wrong source archive")
    return manifest
