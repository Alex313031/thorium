# Copyright (c) 2024 Alex313031 and gz83.

"""
This file is the equivalent of clean.sh in the parent directory, but it directly
deletes //out/thorium and unneeded PGO files.
"""

import os
import shutil
import sys
import subprocess

def fail(msg):
    print(f"{sys.argv[0]}: {msg}", file=sys.stderr)
    sys.exit(111)


def try_run(command):
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError:
        fail(f"Failed {command}")


def clean_files(directory):
    for filename in os.listdir(directory):
        file_path = os.path.join(directory, filename)
        if os.path.isfile(file_path):
            try:
                os.remove(file_path)
                print(f"Removed: {file_path}")
            except Exception as e:
                fail(f"Failed to remove {file_path}: {e}")


def delete_directory(directory):
    if os.path.exists(directory):
        try:
            shutil.rmtree(directory)
            print(f"Removed directory: {directory}")
        except Exception as e:
            fail(f"Failed to remove directory {directory}: {e}")


def display_help():
    print("\nScript to remove unneeded artifacts\n")

if '--help' in sys.argv:
    display_help()
    sys.exit(0)


# Set chromium/src dir from Windows environment variable
cr_src_dir = os.getenv('CR_DIR', r'C:/src/chromium/src')

print("\nCleaning up unneeded artifacts\n")

profiles_dir = os.path.normpath(os.path.join(cr_src_dir, "chrome", "build", "pgo_profiles"))
clean_files(profiles_dir)

thorium_dir = os.path.normpath(os.path.join(cr_src_dir, "out", "thorium"))
delete_directory(thorium_dir)

print("\nDone cleaning artifacts\n")
