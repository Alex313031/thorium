# Copyright (c) 2024 Alex313031 and gz83.

"""
This file is the equivalent of upstream_version.sh in the parent folder, but for native
builds on Windows, we do not need to deal with the vs artifacts hash and linux
sysroot. At the same time, download only the PGO file for win64.
"""

import os
import subprocess
import sys


def fail(msg):
    # Print error message and exit
    print(f"{sys.argv[0]}: {msg}", file=sys.stderr)
    sys.exit(111)


def try_run(command):
    # Execute a command and die on failure
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError:
        fail(f"Failed {command}")


# --help
def display_help():
    print(f"\nScript to check out Chromium tag of current Thorium version.\n")
    print(f"\nNOTE: You may need to run trunk.py before using this script\n")


if "--help" in sys.argv:
    display_help()
    sys.exit(0)


# Set chromium/src dir from Windows environment variable
cr_src_dir = os.getenv("CR_DIR", r"C:/src/chromium/src")


# Set cr_ver
cr_ver = "138.0.7204.193"


print(f"\nCurrent Chromium version is: {cr_ver}\n")
print(f"\nNOTE: Checking out tags/{cr_ver} in {cr_src_dir}\n")

# Change directory to cr_src_dir and run commands
os.chdir(cr_src_dir)

try_run(f"git checkout -f tags/{cr_ver}")

# Commands to run
commands = [
    "git clean -ffd",
    "git clean -ffd",
    "gclient sync --with_branch_heads --with_tags -f -R -D",
    "git clean -ffd",
]

# Run each command with error handling
for cmd in commands:
    try_run(cmd)


print(f"\nChromium tree is checked out at tag: {cr_ver}\n")

print("\nDone!\n")
