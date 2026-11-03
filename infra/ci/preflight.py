#!/usr/bin/env python3
"""Record the actual runner resources without running Thorium products."""

import argparse
import os
from pathlib import Path
import platform
import shutil
import subprocess
import time

from common import configuration, profile_name, summary, write_json


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--path", type=Path, required=True)
    parser.add_argument("--report", type=Path, required=True)
    args = parser.parse_args()
    _, profile = configuration()
    args.path.mkdir(parents=True, exist_ok=True)
    if platform.system() != "Linux" or platform.machine() != "x86_64":
        raise ValueError("Linux x64 runner required")
    tools = ("git", "tar", "zstd", "xz", "python3", "pkg-config", "dpkg-deb", "rpmbuild")
    missing = [name for name in tools if not shutil.which(name)]
    if missing:
        raise ValueError(f"missing build/packaging tools: {missing}")
    disk = shutil.disk_usage(args.path)
    report = {"time": int(time.time()), "profile": profile_name(),
              "host_cpu": profile["host_cpu"], "target_cpu": profile["target_cpu"],
              "system": platform.platform(),
              "python": platform.python_version(), "cpus": os.cpu_count(),
              "image": os.environ.get("ImageVersion", "local"),
              "disk_total_bytes": disk.total, "disk_free_bytes": disk.free,
              "meminfo": Path("/proc/meminfo").read_text(),
              "cpuinfo": subprocess.check_output(["lscpu"], text=True)}
    write_json(args.report, report)
    summary(f"Profile: {profile_name()}, runner: {report['system']}, CPUs: {report['cpus']}, "
            f"available disk space: {disk.free / 2**30:.1f} GiB.")


if __name__ == "__main__":
    main()
