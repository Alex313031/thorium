# Copyright (c) 2024 Alex313031 and gz83.

# This file is the equivalent of make_portable_win.py.


import os
import shutil
import subprocess
import sys
import time
import zipfile


def fail(msg):
    print(f"{sys.argv[0]}: {msg}", file=sys.stderr)
    sys.exit(111)


def try_run(command):
    result = subprocess.run(command, shell=True)
    if result.returncode != 0:
        fail(f"Failed {command}")


def display_help():
    print("\nScript to make a portable Thorium .zip for Windows.\n")
    print("\nPlease place the thorium_mini_installer.exe file in this directory before running.\n")

if '--help' in sys.argv:
    display_help()
    sys.exit(0)


def copy_installer():
    cr_src_dir = os.getenv('CR_DIR', r'C:/src/chromium/src')
    src_path = os.path.normpath(os.path.join(cr_src_dir, 'out', 'thorium', 'thorium_mini_installer.exe'))
    dest_path = os.path.normpath(os.path.join(os.getcwd(), 'thorium_mini_installer.exe'))

    if not os.path.exists(src_path):
        fail(f"thorium_mini_installer.exe not found at {src_path}")

    print(f"Copying thorium_mini_installer.exe from {src_path} to {dest_path}...\n")
    shutil.copy(src_path, dest_path)


def extract_and_copy_files():
    # Extract and copy files
    os.makedirs('./temp/USER_DATA', exist_ok=True)
    try_run('7z x thorium_mini_installer.exe')
    try_run('7z x chrome.7z')
    shutil.move('Chrome-bin', './temp/BIN')
    shutil.copy('./README.win', './temp/README.txt')
    shutil.copy('./THORIUM.BAT', './temp/')
    shutil.copy('./THORIUM_SHELL.BAT', './temp/')

def zip_files():
    # Create zip archive
    with zipfile.ZipFile('thorium_portable.zip', 'w', zipfile.ZIP_DEFLATED) as zf:
        for root, _, files in os.walk('./temp'):
            for file in files:
                file_path = os.path.join(root, file)
                zf.write(file_path, os.path.relpath(file_path, './temp'))

def clean_up():
    # Cleanup extracted files
    try:
        os.remove('chrome.7z')
        shutil.rmtree('temp')
    except FileNotFoundError:
        pass

def main():

    print("\nNOTE: You must place the thorium .exe file in this directory before running.")
    print("   AND you must have 7-Zip installed and in your PATH.")
    print("   Make sure to rename the .zip properly as per the release instructions.")
    print("   AND make sure to edit the THORIUM_SHELL.BAT to match the version number of this release.\n")

    copy_installer()

    input("Press Enter to continue or Ctrl + C to abort.")

    print("Extracting & Copying files from thorium .exe file...\n")
    time.sleep(2)

    extract_and_copy_files()

    print("\nZipping up...\n")
    zip_files()

    print("\nCleaning up...\n")
    time.sleep(2)

    clean_up()

    print("\nDone! Zip is at ./thorium_portable.zip")
    print("Remember to rename it with the version before distributing it.\n")

if __name__ == "__main__":
    main()
