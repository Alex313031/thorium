# Copyright (c) 2024 Alex313031 and gz83.

# This file is the equivalent of make_portable_win.py.


import os
import shutil
import subprocess
import sys
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


if "--help" in sys.argv:
    display_help()
    sys.exit(0)


def copy_installer():
    cr_src_dir = os.getenv("CR_DIR", r"C:/src/chromium/src")
    installer_files = [
        "thorium_mini_installer.exe",
        "thorium_AVX_mini_installer.exe",
        "thorium_AVX2_mini_installer.exe",
        "thorium_SSE3_mini_installer.exe",
        "thorium_SSE4_mini_installer.exe",
    ]

    existing_files = [
        installer_file
        for installer_file in installer_files
        if os.path.exists(os.path.join(os.getcwd(), installer_file))
    ]

    if existing_files:
        print(f"Skipping copy for existing files: {', '.join(existing_files)}\n")
        print("Starting portable version creation.")
        return existing_files[0]

    for installer_file in installer_files:
        src_path = os.path.normpath(
            os.path.join(cr_src_dir, "out", "thorium", installer_file)
        )
        dest_path = os.path.normpath(os.path.join(os.getcwd(), installer_file))

        if os.path.exists(src_path):
            print(f"Copying {installer_file} from {src_path} to {dest_path}...\n")
            shutil.copy(src_path, dest_path)
            return installer_file

    fail("No installer file found.")


def extract_and_copy_files(installer_name):
    os.makedirs("./temp/USER_DATA", exist_ok=True)
    try_run(f"7z x {installer_name}")
    try_run("7z x chrome.7z")
    shutil.move("Chrome-bin", "./temp/BIN")
    shutil.copy("./README.win", "./temp/README.txt")
    shutil.copy("./THORIUM.BAT", "./temp/")
    shutil.copy("./THORIUM_SHELL.BAT", "./temp/")


def zip_files(installer_name):
    version = "130.0.6723.174"

    if "AVX2" in installer_name:
        zip_filename = f"Thorium_AVX2_{version}.zip"
    elif "AVX" in installer_name:
        zip_filename = f"Thorium_AVX_{version}.zip"
    elif "SSE3" in installer_name:
        zip_filename = f"Thorium_SSE3_{version}.zip"
    elif "SSE4" in installer_name:
        zip_filename = f"Thorium_SSE4_{version}.zip"
    else:
        zip_filename = f"thorium_portable.zip"

    with zipfile.ZipFile(zip_filename, "w", zipfile.ZIP_DEFLATED) as zf:
        for root, _, files in os.walk("./temp"):
            if not files:
                zf.write(root, os.path.relpath(root, "./temp"))
            for file in files:
                file_path = os.path.join(root, file)
                zf.write(file_path, os.path.relpath(file_path, "./temp"))

    print(f"Created zip archive: {zip_filename}")


def clean_up():
    try:
        os.remove("chrome.7z")
        shutil.rmtree("temp")
    except FileNotFoundError:
        pass


def main():
    print(
        "\nNOTE: Place the Thorium .exe in this directory and ensure 7-Zip is in PATH.\n"
    )

    installer_name = copy_installer()
    input("Press Enter to continue or Ctrl + C to abort.")

    print("Extracting files from installer file...\n")
    extract_and_copy_files(installer_name)

    print("\nZipping up...\n")
    zip_files(installer_name)

    print("\nCleaning up...\n")
    clean_up()

    print("\nDone! Zip is at //infra/portable.\n")


if __name__ == "__main__":
    main()
