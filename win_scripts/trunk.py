# Copyright (c) 2024 Alex313031, midzer and gz83.

"""
This file is the equivalent of trunk.sh in the parent folder, but for native
builds on Windows, we do not need to deal with the vs artifacts hash.
"""

import os
import shutil
import subprocess
import sys


def fail(msg):
    # Print error message and exit
    print(f"{sys.argv[0]}: {msg}", file=sys.stderr)
    sys.exit(111)


def try_run(command):
    # Execute a command and exit on failure
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError:
        fail(f"Failed {command}")


def safe_rmtree(path):
    # Remove a directory if it exists
    path = os.path.normpath(path)
    if os.path.exists(path):
        print(f"Removing: {path}")
        shutil.rmtree(path)


# --help
def display_help():
    print(f"\nScript to Rebase/Sync the Chromium repo.\n")


if "--help" in sys.argv:
    display_help()
    sys.exit(0)


# Set chromium/src dir from Windows environment variable
cr_src_dir = os.getenv("CR_DIR", r"C:/src/chromium/src")
# Thorium-specific directory that need to be deleted
pak_dir = os.path.normpath(os.path.join(cr_src_dir, "third_party", "pak"))


def main():

    print("\nScript to Rebase/Sync the Chromium repo.\n")
    print(f"Rebasing/Syncing and running hooks in {cr_src_dir}\n")

    # Change directory to cr_src_dir and run commands
    os.chdir(cr_src_dir)

    # Remove Thorium-specific directory if it exists
    safe_rmtree(pak_dir)

    # Commands to run
    commands = [
        "cd v8 && git restore . && git clean -ffd",
        "cd third_party/devtools-frontend/src && git restore . && git clean -ffd",
        "cd third_party/ffmpeg && git restore . && git clean -ffd",
        "git checkout -f origin/main",
        "git clean -ffd",
        "git clean -ffd",
        "git rebase-update",
        "git fetch --tags",
        "gclient sync --with_branch_heads --with_tags -f -R -D",
        "git clean -ffd",
        "gclient runhooks",
    ]

    # Run each command with error handling
    for cmd in commands:
        try_run(cmd)

    print("\nDone! You can now run version.py.\n")


if __name__ == "__main__":
    main()
