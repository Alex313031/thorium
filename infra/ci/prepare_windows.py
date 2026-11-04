#!/usr/bin/env python3
"""Prepare the full Chromium export for a native Windows build."""

import argparse
from pathlib import Path
import shutil

from common import configuration, download, profile_name, sha256, summary, unpack, write_json
from prepare_source import configure_tree, install_dependencies
from windows_archive import make_directory
from windows_host import inspect, require_runner


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work-root", type=Path, required=True)
    parser.add_argument("--cache", type=Path, required=True)
    args = parser.parse_args()
    require_runner()
    work, cache = args.work_root.resolve(), args.cache.resolve()
    sources, profile = configuration()
    if profile["target_os"] != "win" or cache.is_relative_to(work):
        raise ValueError("Windows profile and an external download cache are required")
    host = inspect()
    make_directory(work)
    archive = download(sources["chromium"], cache)
    if archive.stat().st_size != sources["chromium"]["size_bytes"] or sha256(archive) != sources["chromium"]["sha256"]:
        raise ValueError("source archive does not match its lock")
    unpack(archive, work)
    (work / sources["chromium"]["root"]).rename(work / "src")
    archive.unlink()
    # Replace only Linux host binaries bundled in the official export. Source
    # files, including case-distinct names and symlinks, remain intact.
    for relative in ("third_party/cpython3/host", "third_party/llvm-build/Release+Asserts",
                     "third_party/rust-toolchain", "third_party/llvm-libclang", "third_party/siso/cipd"):
        path = work / "src" / relative
        if path.exists():
            shutil.rmtree(path)
    write_json(work / "dependencies.json", install_dependencies(work, cache, sources))
    write_json(work / "windows-host.json", host)
    configure_tree(work, sources, profile)
    summary(f"Prepared {profile_name()}: full tarball, native Windows tools, local Siso.")


if __name__ == "__main__":
    main()
