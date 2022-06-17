#!/bin/bash

# Copyright (c) 2022 Alex313031.

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
	printf "${bold}${YEL}Script to build Thorium .appimage on Linux.${c0}\n" &&
	printf "${bold}${YEL}Please place the Thorium .deb file in this directory before running.${c0}\n" &&
	printf "\n"
}

case $1 in
	--help) displayHelp; exit 0;;
esac

printf "\n" &&
printf "${bold}${RED}NOTE: You must place the Thorium .deb file in this directory before running.${c0}\n" &&
printf "${YEL}Extracting & Copying files from Thorium .deb package...\n" &&
printf "${c0}\n" &&

# Extract data.tar.xz
ar x thorium-browser* &&
tar xf data.tar.xz &&
mkdir thorium &&
cp -r opt/chromium.org/thorium-unstable/* thorium/ &&
cp -r usr/bin/chromedriver thorium/ &&
cp -r usr/bin/pak thorium/ &&
rm -r thorium/cron &&
rm -r thorium/thorium-browser &&
rm -r thorium/thorium-browser-unstable &&

printf "\n" &&
printf "${YEL}Building .Appimage using Thorium.yml...\n" &&
printf "${c0}\n" &&

# Build Appimage
chmod +x pkg2appimage &&
./pkg2appimage Thorium.yml &&

printf "\n" &&
printf "${YEL}Cleaning up...\n" &&
printf "${c0}\n" &&

# Cleanup
rm -r opt &&
rm -r etc &&
rm -r usr &&
rm -r control.tar.xz &&
rm -r data.tar.xz &&
rm -r debian-binary &&
rm -r Thorium/ &&
rm -r thorium/ &&

printf "${GRE}Done! ${YEL}Appimage at //out/thorium*.Appimage\n" &&
printf "\n" &&
tput sgr0
