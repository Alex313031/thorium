#!/bin/bash

# Copyright (c) 2024 Alex313031.

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

printf "\n" &&
printf "${bold}${RED}NOTE: You must place the Thorium .exe file in this directory before running.${c0}\n" &&
printf "${bold}${RED}   AND you must have 7-Zip installed and in your PATH.${c0}\n" &&
printf "\n" &&
printf "${bold}${RED} - Make sure to rename the .zip properly as per https://github.com/Alex313031/thorium/discussions/28#discussioncomment-3031799 ${c0}\n" &&
printf "${bold}${RED}   AND make sure to edit the THORIUM_SHELL.BAT to match the version number of this release.${c0}\n" &&
printf "${YEL}\n" &&

read -p "Press Enter to continue or Ctrl + C to abort."
printf "\n" &&

printf "${YEL}Extracting & Copying files from Thorium .exe file...\n" &&
printf "${c0}\n" &&

sleep 2 &&

# Extract data.tar.xz
mkdir -v -p ./temp &&
mkdir -v -p ./temp/USER_DATA &&
7z x thorium_mini_installer.exe &&
7z x chrome.7z &&
mv -v Chrome-bin ./temp/BIN &&
cp -r -v ./README.win temp/README.txt &&
cp -r -v ./THORIUM.BAT temp/ &&
cp -r -v ./THORIUM_SHELL.BAT temp/ &&

printf "\n" &&
printf "${YEL}Zipping up...\n" &&
printf "${c0}\n" &&

# Build zip
cd temp; zip -r ../thorium_portable.zip * &&

printf "\n" &&
printf "${YEL}Cleaning up...\n" &&
printf "${c0}\n" &&

sleep 2 &&

# Cleanup
cd .. &&
rm -r -v chrome.7z &&
rm -r -v temp &&

printf "\n" &&
printf "${GRE}Done! ${YEL}Zip at ./thorium_portable.zip\n - Remember to rename it with the version before distributing it.\n" &&
printf "\n" &&
tput sgr0
