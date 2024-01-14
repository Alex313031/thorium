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
	printf "${bold}${GRE}Script to extract Thorium .AppImage on Linux.${c0}\n" &&
	printf "${bold}${YEL}Please run build_appimage.sh in this directory before running.${c0}\n" &&
	printf "\n"
}
case $1 in
	--help) displayHelp; exit 0;;
esac

printf "\n" &&
printf "${bold}${RED}NOTE: You must run build_appimage.sh in this directory before running.${c0}\n" &&
printf "${YEL}Extracting Thorium*.Appimage...\n" &&
printf "${c0}\n" &&

sleep 2 &&

# Extract data.tar
./Thorium_Browser_119.0.6045.214_AVX2.AppImage --appimage-extract &&

printf "\n" &&
printf "${YEL}Renaming squashfs-root to Thorium_squashfs-root...\n" &&
printf "${c0}\n" &&

# Rename
mv -v squashfs-root Thorium_squashfs-root &&

printf "\n" &&
printf "${GRE}Done! ${YEL}Extracted contents at //out/Thorium_squashfs-root/\n" &&
printf "\n" &&
tput sgr0
