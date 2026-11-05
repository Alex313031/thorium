#!/usr/bin/env python3
"""Prepare a full Chromium export with native macOS dependencies."""

import argparse
from pathlib import Path
import shutil

from common import configuration, download, profile_name, sha256, summary, unpack, write_json
from macos_host import WORK_ROOT, inspect, require_runner
from prepare_source import configure_tree, install_dependencies


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work-root", type=Path, required=True)
    parser.add_argument("--cache", type=Path, required=True)
    args = parser.parse_args()
    require_runner()
    work, cache = args.work_root.resolve(), args.cache.resolve()
    sources, profile = configuration()
    if work != WORK_ROOT or work.exists() or not work.parent.is_mount() or cache.is_relative_to(work):
        raise ValueError("a fresh work root on the mounted build volume and an external cache are required")
    host = inspect()
    archive = download(sources["chromium"], cache)
    if archive.stat().st_size != sources["chromium"]["size_bytes"] or sha256(archive) != sources["chromium"]["sha256"]:
        raise ValueError("source archive does not match its lock")
    print("Extracting the full Chromium tarball on case-sensitive APFS", flush=True)
    unpack(archive, work)
    (work / sources["chromium"]["root"]).rename(work / "src")
    archive.unlink()
    # Replace Linux host tool binaries, retaining every upstream source file.
    for relative in ("third_party/cpython3/host", "third_party/llvm-build/Release+Asserts",
                     "third_party/rust-toolchain", "third_party/llvm-libclang", "third_party/siso/cipd",
                     "third_party/ninja"):
        path = work / "src" / relative
        if path.exists():
            shutil.rmtree(path)
    write_json(work / "dependencies.json", install_dependencies(work, cache, sources))
    write_json(work / "macos-host.json", host)
    configure_tree(work, sources, profile)
    summary(f"Prepared {profile_name()}: full tarball, native macOS tools, local Siso.")


if __name__ == "__main__":
    main()
