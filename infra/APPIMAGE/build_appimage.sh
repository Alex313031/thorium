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
	printf "${bold}${GRE}Script to build Thorium .AppImage on Linux.${c0}\n" &&
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

rm -r -f ./temp/ &&

sleep 2 &&

# Extract data.tar.xz
mkdir -v ./temp &&
ar xv *.deb &&
tar xvf ./data.tar.xz &&
cp -r -v ./opt/chromium.org/thorium/* ./temp/ &&
cp -r -v ./usr/bin/pak temp/ &&
cp -r -v ./files/product_logo_512.png ./temp/ &&
cp -r -v ./files/product_logo_22.png ./temp/ &&
cp -r -v ./files/thorium-shell ./temp/ &&
rm -r -v ./temp/cron &&
rm -r -v ./temp/thorium-browser &&

printf "\n" &&
printf "${YEL}Building .AppImage using Thorium.yml...\n" &&
printf "${c0}\n" &&

sleep 2 &&

# Build appimage
# chmod +x pkg2appimage &&
./pkg2appimage Thorium.yml &&

printf "\n" &&
printf "${YEL}Cleaning up...\n" &&
printf "${c0}\n" &&

sleep 2 &&

# Cleanup
rm -r -v -f ./opt &&
rm -r -v -f ./etc &&
rm -r -v -f ./usr &&
rm -r -v -f ./control.tar.xz &&
rm -r -v -f ./data.tar.xz &&
rm -r -v -f ./debian-binary &&
rm -r -v -f ./Thorium/ &&
rm -r -v -f ./temp/ &&

printf "\n" &&
printf "${GRE}Done! ${YEL}AppImage at //out/Thorium*.AppImage\n" &&
printf "\n" &&
tput sgr0
