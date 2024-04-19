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
	printf "${bold}${GRE}Script to make a portable Thorium .zip for Linux.${c0}\n" &&
	printf "${bold}${YEL}Please place the Thorium .deb file in this directory before running.${c0}\n" &&
	printf "${bold}${YEL}Use the --arm flag for Raspberry Pi builds.${c0}\n" &&
	printf "\n"
}
case $1 in
	--help) displayHelp; exit 0;;
esac

makeARM () {
	printf "\n" &&
	printf "${bold}${RED}NOTE: You must place the Thorium .deb file in this directory before running.${c0}\n" &&
	printf "${bold}${RED}   AND you must have p7zip and zip installed.${c0}\n" &&
	printf "\n" &&

	read -p "Press Enter to continue or Ctrl + C to abort."
	printf "\n" &&

	printf "${YEL}Extracting & Copying files from Thorium .deb package (arm64 version)...\n" &&
	printf "${c0}\n" &&

	sleep 2 &&

	BASENAME=$(basename -- *.deb) &&
	FILENAME="${BASENAME%.deb}" &&

	export FILENAME &&

	# Extract data.tar.xz
	mkdir -v -p ./temp &&
	ar xv *.deb &&
	tar xvf ./data.tar.xz &&
	cp -r -v ./opt/chromium.org/thorium/* ./temp/ &&
	rm -r -v ./temp/cron &&
	rm -r -v ./temp/thorium-browser &&
	cp -r -v ./usr/bin/pak temp/ &&
	cp -r -v ./README.linux temp/README.txt &&
	cp -r -v ./THORIUM-PORTABLE.sh temp/ &&
	cp -r -v ./thorium-portable.desktop temp/ &&
	cp -r -v ./THORIUM-SHELL.sh temp/ &&
	cp -r -v ./thorium-shell.desktop temp/ &&

	printf "\n" &&
	printf "${YEL}Zipping up...\n" &&
	printf "${c0}\n" &&

	# Build zip
	cd temp; zip -r ../${FILENAME}.zip * &&

	printf "\n" &&
	printf "${YEL}Cleaning up...\n" &&
	printf "${c0}\n" &&

	sleep 2 &&

	# Cleanup
	cd .. &&
	rm -r -v ./opt &&
	rm -r -v ./etc &&
	rm -r -v ./usr &&
	rm -r -v ./control.tar.xz &&
	rm -r -v ./data.tar.xz &&
	rm -r -v ./debian-binary &&
	rm -r -v ./temp &&

	printf "\n" &&
	printf "${GRE}Done! ${YEL}Zip at ./${FILENAME}.zip\n" &&
	printf "\n" &&
	tput sgr0
}
case $1 in
	--arm) makeARM; exit 0;;
esac

printf "\n" &&
printf "${bold}${RED}NOTE: You must place the Thorium .deb file in this directory before running.${c0}\n" &&
printf "${bold}${RED}   AND you must have p7zip and zip installed.${c0}\n" &&
printf "\n" &&

read -p "Press Enter to continue or Ctrl + C to abort."
printf "\n" &&

printf "${YEL}Extracting & Copying files from Thorium .deb package...\n" &&
printf "${c0}\n" &&

sleep 2 &&

BASENAME=$(basename -- *.deb) &&
FILENAME="${BASENAME%.deb}" &&

export FILENAME &&

# Extract data.tar.xz
mkdir -v -p ./temp &&
ar xv *.deb &&
tar xvf ./data.tar.xz &&
cp -r -v ./opt/chromium.org/thorium/* ./temp/ &&
rm -r -v ./temp/cron &&
rm -r -v ./temp/thorium-browser &&
cp -r -v ./usr/bin/pak temp/ &&
cp -r -v ./README.linux temp/README.txt &&
cp -r -v ./THORIUM-PORTABLE temp/ &&
cp -r -v ./thorium-portable.desktop temp/ &&
cp -r -v ./THORIUM-SHELL temp/ &&
cp -r -v ./thorium-shell.desktop temp/ &&

printf "\n" &&
printf "${YEL}Zipping up...\n" &&
printf "${c0}\n" &&

# Build zip
cd temp; zip -r ../${FILENAME}.zip * &&

printf "\n" &&
printf "${YEL}Cleaning up...\n" &&
printf "${c0}\n" &&

sleep 2 &&

# Cleanup
cd .. &&
rm -r -v ./opt &&
rm -r -v ./etc &&
rm -r -v ./usr &&
rm -r -v ./control.tar.xz &&
rm -r -v ./data.tar.xz &&
rm -r -v ./debian-binary &&
rm -r -v ./temp &&

printf "\n" &&
printf "${GRE}Done! ${YEL}Zip at ./${FILENAME}.zip\n" &&
printf "\n" &&
tput sgr0
