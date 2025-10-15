# Copyright (c) 2024 Alex313031 and gz83.

"""
This file is used to sync commits in Chromium's Tip-of-tree branch and does not
contain any Thorium-specific files.
"""


import os
import subprocess
import sys


# Error handling function
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
    print(f"\nScript to Rebase/Sync Chromium repo to Tip of Tree.\n")


if "--help" in sys.argv:
    display_help()
    sys.exit(0)


# Set chromium/src dir from Windows environment variable
cr_src_dir = os.getenv("CR_DIR", r"C:/src/chromium/src")


def main():

    print("\nScript to Rebase/Sync Chromium repo to Tip of Tree.\n")
    print(
        f"Rebasing/Syncing to `origin/main` and running hooks in {cr_src_dir}\n")

    # Change directory to cr_src_dir and run commands
    os.chdir(cr_src_dir)

    # Commands to run
    commands = [
        "cd v8 && git restore . && git clean -ffd",
        "cd third_party/devtools-frontend/src && git restore . && git clean -ffd",
        "cd third_party/ffmpeg && git restore . && git clean -ffd",
        "git checkout -f origin/main",
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

    print("\nDone!\n")


if __name__ == "__main__":
    main()
