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
    deleted = False
    for filename in os.listdir(directory):
        file_path = os.path.join(directory, filename)
        if os.path.isfile(file_path):
            try:
                os.remove(file_path)
                deleted = True
                print(f"Removed: {file_path}")
            except Exception as e:
                fail(f"Failed to remove {file_path}: {e}")
    return deleted


def delete_directory(directory):
    if os.path.exists(directory):
        try:
            shutil.rmtree(directory)
            print(f"Removed directory: {directory}")
            return True
        except Exception as e:
            fail(f"Failed to remove directory {directory}: {e}")
    return False


def display_help():
    print("\nScript to remove unneeded artifacts\n")


if "--help" in sys.argv:
    display_help()
    sys.exit(0)


def main():
    cr_src_dir = os.getenv("CR_DIR", r"C:\src\chromium\src")
    profiles_dir = os.path.normpath(
        os.path.join(cr_src_dir, "chrome", "build", "pgo_profiles")
    )
    thorium_dir = os.path.normpath(os.path.join(cr_src_dir, "out", "thorium"))

    cleanup_needed = any(
        [
            (
                os.path.isdir(profiles_dir)
                and any(
                    os.path.isfile(os.path.join(profiles_dir, filename))
                    for filename in os.listdir(profiles_dir)
                )
            ),
            os.path.exists(thorium_dir),
        ]
    )

    if cleanup_needed:
        print("\nCleaning up unneeded artifacts\n")

        files_deleted = (
            clean_files(profiles_dir) if os.path.isdir(profiles_dir) else False
        )
        dir_deleted = (
            delete_directory(thorium_dir) if os.path.exists(
                thorium_dir) else False
        )

        if files_deleted or dir_deleted:
            print("\nDone cleaning unneeded artifacts\n")


if __name__ == "__main__":
    main()
