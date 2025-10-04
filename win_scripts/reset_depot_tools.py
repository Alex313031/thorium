# Copyright (c) 2024 Alex313031, midzer and gz83.

"""
This file is the equivalent of reset_depot_tools.py in the parent folder, but we
do not need to deal with the .vpython_cipd_cache.
This file may prompt "Access denied" and other prompts during the operation, but
in fact it seems that the files we need to delete have been deleted.
"""

# TODO(gz83): Suppress false positives during operation?

import os
import subprocess
import sys


def fail(msg):
    print(f"{sys.argv[0]}: {msg}", file=sys.stderr)
    sys.exit(111)


def try_run(command):
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError:
        fail(f"Failed {command}")


def remove(item_path):
    if os.path.exists(item_path):
        if os.path.isdir(item_path):
            try:
                # Try to unlock and delete the directory
                unlock_and_delete(item_path)
                print(f"removed '{item_path}'")
            except PermissionError as e:
                print(f"Failed to remove '{item_path}': {e}")
        else:
            os.remove(item_path)
            print(f"removed '{item_path}'")


def unlock_and_delete(path):
    """Attempts to unlock and delete a directory using Windows cmd commands."""
    # Use the Windows command line tools to unlock the directory
    try:
        # Use the command 'del' to delete all files recursively
        subprocess.run(f'del /S /Q "{path}\\*"', shell=True, check=True)
        # Use the command 'rmdir' to delete the directory
        subprocess.run(f'rmdir /S /Q "{path}"', shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Failed to unlock and delete directory '{path}' via CMD: {e}")
        raise PermissionError(
            f"Failed to unlock and delete directory '{path}' via CMD: {e}"
        )


def display_help():
    print("\nScript to reset depot_tools on Windows.\n")
    print("This will remove depot_tools, .gsutil, and .vpython-root")
    print("from your disk, and then re-clone depot_tools.")
    print("\n")


if "--help" in sys.argv:
    display_help()
    sys.exit(0)


depot_tools_dir = os.getenv("DEPOT_TOOLS_DIR", r"C:\src\depot_tools")
gsutil_dir = os.path.expandvars(
    os.getenv("GSUTIL_DIR", r"%USERPROFILE%\.gsutil"))
vpython_root_dir = os.path.expandvars(
    os.getenv("VPYTHON_ROOT_DIR", r"%LOCALAPPDATA%\.vpython-root")
)

print("\nRemoving depot_tools, etc\n")

remove(depot_tools_dir)
remove(gsutil_dir)
remove(vpython_root_dir)

print("\nRe-clone depot_tools\n")

os.chdir(os.path.dirname(depot_tools_dir))
try_run(f"git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git")

print(
    f"\nCompleted. You can now use the depot_tools installed at: {depot_tools_dir}\n")
print("\nYou can now run trunk.py\n")
