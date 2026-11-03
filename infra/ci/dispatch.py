#!/usr/bin/env python3
"""Validate a manual request and select independent Linux build chains."""

import argparse
import json

from common import configuration, output, profiles, summary


def select(requested, jobs, minutes, parallel, resume_run, resume_artifact):
    if not 1 <= jobs <= 8 or not 10 <= minutes <= 240 or not 1 <= parallel <= 6:
        raise ValueError("jobs must be 1..8, minutes 10..240, parallel profiles 1..6")
    if bool(resume_run) != bool(resume_artifact):
        raise ValueError("resume requires both run ID and artifact ID")
    if resume_run and (not resume_run.isascii() or not resume_run.isdecimal() or
                       not resume_artifact.isascii() or not resume_artifact.isdecimal() or
                       int(resume_run) < 1 or int(resume_artifact) < 1):
        raise ValueError("resume requires positive numeric run and artifact IDs")
    if requested == "all":
        if resume_run:
            raise ValueError("resume selects exactly one profile; all cannot share a checkpoint")
        selected = list(profiles())
    else:
        selected = ["linux-arm64" if requested == "arm64" else "linux-x64-" + requested]
    for name in selected:
        configuration(name)
    return {"include": [{"profile": name} for name in selected]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--profile", required=True,
                        choices=("avx", "avx2", "avx512", "sse3", "sse4", "arm64", "all"))
    parser.add_argument("--jobs", type=int, required=True)
    parser.add_argument("--minutes", type=int, required=True)
    parser.add_argument("--parallel", type=int, required=True)
    parser.add_argument("--resume-run", default="")
    parser.add_argument("--resume-artifact", default="")
    args = parser.parse_args()
    matrix = select(args.profile, args.jobs, args.minutes, args.parallel,
                    args.resume_run, args.resume_artifact)
    output("matrix", json.dumps(matrix))
    summary("Selected Linux profiles: " + ", ".join(row["profile"] for row in matrix["include"]))


if __name__ == "__main__":
    main()
