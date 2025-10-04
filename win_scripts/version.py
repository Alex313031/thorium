# Copyright (c) 2024 Alex313031 and gz83.

"""
This file is the equivalent of version.sh in the parent folder, but for native
builds on Windows, we do not need to deal with the vs artifacts hash and linux
sysroot. At the same time, download only the PGO file for win64.
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
    # Execute a command and die on failure
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError:
        fail(f"Failed {command}")


def copy(src, dst):
    # Copy a file and print verbose output like cp -v
    try:
        print(f"Copying {src} to {dst}")
        shutil.copy(src, dst)
    except FileNotFoundError as e:
        fail(f"File copy failed: {e}")


# --help
def display_help():
    print(f"\nScript to check out Chromium tag of current Thorium version.\n")
    print(f"\nNOTE: You may need to run trunk.py before using this script\n")


if "--help" in sys.argv:
    display_help()
    sys.exit(0)


# Set chromium/src dir from Windows environment variable
cr_src_dir = os.getenv("CR_DIR", r"C:/src/chromium/src")
# Set Thorium dir from Windows environment variable
thor_src_dir = os.path.expandvars(
    os.getenv("THOR_DIR", r"%USERPROFILE%/thorium"))


# Set thor_ver
thor_ver = "138.0.7204.193"


print(f"\nCurrent Thorium version is: {thor_ver}\n")
print(f"\nNOTE: Checking out tags/{thor_ver} in {cr_src_dir}\n")

# Change directory to cr_src_dir and run commands
os.chdir(cr_src_dir)

try_run(f"git checkout -f tags/{thor_ver}")

# Copy files using shutil
copy(
    os.path.normpath(os.path.join(thor_src_dir, "thorium-libjxl/src/DEPS")),
    os.path.normpath(os.path.join(cr_src_dir, "DEPS")),
)
copy(
    os.path.normpath(os.path.join(
        thor_src_dir, "thorium-libjxl/src/.gitmodules")),
    os.path.normpath(os.path.join(cr_src_dir, ".gitmodules")),
)
copy(
    os.path.normpath(
        os.path.join(thor_src_dir, "thorium-libjxl/src/third_party/.gitignore")
    ),
    os.path.normpath(os.path.join(cr_src_dir, "third_party/.gitignore")),
)

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

print(f"\nChromium tree is checked out at tag: {thor_ver}\n")

print("\nDone! You can now run setup.py\n")
