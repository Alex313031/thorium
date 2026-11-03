#!/usr/bin/env python3
"""Export or restore an immutable full build checkpoint, with an external digest."""

import argparse
from pathlib import Path
import subprocess
import time

from common import (input_identity, output, read_json, sha256, validate_manifest,
                    write_json)


def pack(work, destination):
    work, destination = work.resolve(), destination.resolve()
    if destination.is_relative_to(work):
        raise ValueError("checkpoint archive must be outside its input tree")
    manifest = validate_manifest(work)
    if manifest["status"] != "checkpointed":
        raise ValueError("only a prepared or deliberately paused tree can be checkpointed")
    destination.mkdir(parents=True, exist_ok=False)
    payload = destination / "build.tar.zst"
    temporary = destination / "build.tar.zst.partial"
    start = time.monotonic()
    subprocess.run(["tar", "--format=posix", "--use-compress-program=zstd -T2 -3", "-cf", str(temporary),
                    "-C", str(work), "."], check=True)
    subprocess.run(["zstd", "-t", str(temporary)], check=True)
    temporary.replace(payload)
    write_json(destination / "checkpoint.json", {
        "schema_version": 1, "identity": manifest["identity"],
        "status": manifest["status"], "stage": manifest["stage"],
        "phase_index": manifest["phase_index"], "work_root": str(work),
        "payload": payload.name, "size_bytes": payload.stat().st_size,
        "sha256": sha256(payload), "archive_seconds": time.monotonic() - start,
    })


def restore(archive_dir, work):
    archive_dir, work = archive_dir.resolve(), work.resolve()
    header = read_json(archive_dir / "checkpoint.json")
    if header.get("schema_version") != 1 or header.get("identity") != input_identity():
        raise ValueError("checkpoint schema or input identity does not match")
    if header.get("work_root") != str(work) or header.get("payload") != "build.tar.zst":
        raise ValueError("checkpoint path does not match")
    if header.get("status") != "checkpointed":
        raise ValueError("this artifact is not a resumable checkpoint")
    payload = archive_dir / header["payload"]
    if payload.stat().st_size != header["size_bytes"] or sha256(payload) != header["sha256"]:
        raise ValueError("checkpoint archive is truncated or corrupted")
    if work.exists():
        raise ValueError("restore requires a fresh work root")
    work.mkdir(parents=True)
    # Only our own digest-checked artifacts from a matching, trusted run enter
    # this path. GNU tar preserves all modes, links, nanosecond timestamps and
    # hidden Siso state instead of uploading loose files.
    subprocess.run(["tar", "--zstd", "-xf", str(payload), "--no-same-owner", "--same-permissions",
                    "-C", str(work)], check=True)
    manifest = validate_manifest(work)
    if any(manifest[key] != header[key] for key in ("stage", "phase_index", "status")):
        raise ValueError("checkpoint header and payload disagree")
    payload.unlink()
    output("restored-stage", manifest["stage"])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("pack", "restore"))
    parser.add_argument("--work-root", required=True, type=Path)
    parser.add_argument("--archive-dir", required=True, type=Path)
    args = parser.parse_args()
    if args.command == "pack":
        pack(args.work_root, args.archive_dir)
    else:
        restore(args.archive_dir, args.work_root)


if __name__ == "__main__":
    main()
