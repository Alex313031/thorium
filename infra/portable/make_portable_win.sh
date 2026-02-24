#!/bin/bash

# Copyright (c) 2026 Alex313031.

YEL='\033[1;33m' # Yellow
RED='\033[1;31m' # Red
GRE='\033[1;32m' # Green
c0=$'\033[0m' # Reset Text
bold=$'\033[1m' # Bold Text
underline=$'\033[4m' # Underline Text

# Error handling
yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "${RED}Failed $*"; }

# --help
displayHelp () {
	printf "\n" &&
	printf "${bold}${GRE}Script to make a portable Thorium .zip for Windows.${c0}\n" &&
	printf "${bold}${YEL}Please place the thorium_mini_installer.exe file in this directory before running.${c0}\n" &&
	printf "\n"
}
case $1 in
	--help) displayHelp; exit 0;;
esac

# Detect which installer is present and set output zip name accordingly
VERSION="138.0.7204.303"
INSTALLER_NAME=""
ZIP_NAME=""

INSTALLER_LIST=(
	"thorium_AVX2_mini_installer.exe"
	"thorium_AVX_mini_installer.exe"
	"thorium_SSE4_mini_installer.exe"
	"thorium_SSE3_mini_installer.exe"
	"thorium_mini_installer.exe"
)

for f in "${INSTALLER_LIST[@]}"; do
	if [ -f "./$f" ]; then
		INSTALLER_NAME="$f"
		break
	fi
done

if [ -z "$INSTALLER_NAME" ]; then
	die "${RED}No installer file found. Place a thorium_*_mini_installer.exe in this directory."
fi

if [[ "$INSTALLER_NAME" == *"AVX2"* ]]; then
	ZIP_NAME="Thorium_AVX2_${VERSION}.zip"
elif [[ "$INSTALLER_NAME" == *"AVX"* ]]; then
	ZIP_NAME="Thorium_AVX_${VERSION}.zip"
elif [[ "$INSTALLER_NAME" == *"SSE4"* ]]; then
	ZIP_NAME="Thorium_SSE4_${VERSION}.zip"
elif [[ "$INSTALLER_NAME" == *"SSE3"* ]]; then
	ZIP_NAME="Thorium_SSE3_${VERSION}.zip"
else
	ZIP_NAME="thorium_portable.zip"
fi

printf "\n" &&
printf "${bold}${RED}NOTE: You must place the Thorium .exe file in this directory before running.${c0}\n" &&
printf "${bold}${RED}   AND you must have 7-Zip installed and in your PATH.${c0}\n" &&
printf "\n" &&
printf "${bold}${YEL}Detected installer: ${GRE}${INSTALLER_NAME}${c0}\n" &&
printf "${bold}${YEL}Output zip will be: ${GRE}${ZIP_NAME}${c0}\n" &&
printf "${YEL}\n" &&

read -p "Press Enter to continue or Ctrl + C to abort."
printf "\n" &&

printf "${YEL}Extracting & Copying files from Thorium .exe file...\n" &&
printf "${c0}\n" &&

sleep 2 &&

# Extract installer
mkdir -v -p ./temp &&
mkdir -v -p ./temp/USER_DATA &&
7z x "$INSTALLER_NAME" &&
7z x chrome.7z &&
mv -v Chrome-bin ./temp/BIN &&
cp -r -v ./README.win temp/README.txt &&
cp -r -v ./THORIUM.BAT temp/ &&
cp -r -v ./THORIUM_SHELL.BAT temp/ &&

printf "\n" &&
printf "${YEL}Zipping up...\n" &&
printf "${c0}\n" &&

# Build zip
cd temp; zip -r "../${ZIP_NAME}" * &&

printf "\n" &&
printf "${YEL}Cleaning up...\n" &&
printf "${c0}\n" &&

sleep 2 &&

# Cleanup
cd .. &&
rm -r -v chrome.7z &&
rm -r -v temp &&

printf "\n" &&
printf "${GRE}Done! ${YEL}Zip at ./${ZIP_NAME}\n" &&
printf "\n" &&
tput sgr0
