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
	printf "${bold}${GRE}Script to copy Thorium ARM BUILD.gn file over the Chromium source tree.${c0}\n" &&
	printf "\n"
}
case $1 in
	--help) displayHelp; exit 0;;
esac

printf "\n" &&
printf "${YEL}Creating build output directory...\n" &&
tput sgr0 &&

mkdir -v -p $HOME/chromium/src/out/thorium/ &&
printf "\n" &&

printf "${YEL}Copying BUILD.gn...\n" &&
tput sgr0 &&

cp -r -v ./build/* $HOME/chromium/src/build/ &&
cp -r -v ./media/* $HOME/chromium/src/media/ &&
cp -r -v ./third_party/* $HOME/chromium/src/third_party/ &&

printf "${GRE}Done!\n" &&
printf "\n" &&

printf "${GRE}Enjoy Thorium on ARM!\n" &&
printf "\n" &&
tput sgr0
