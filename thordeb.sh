#!/bin/bash

# Copyright (c) 2023 Alex313031.

YEL='\033[1;33m' # Yellow
CYA='\033[1;96m' # Cyan
RED='\033[1;31m' # Red
GRE='\033[1;32m' # Green
c0='\033[0m' # Reset Text
bold='\033[1m' # Bold Text
underline='\033[4m' # Underline Text

# Error handling
yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "${RED}Failed $*"; }

# --help
displayHelp () {
	printf "\n" &&
	printf "${bold}${GRE}Script to build Thorium .deb package on Linux.${c0}\n" &&
	printf "${underline}${YEL}Usage: ${c0}thordeb.sh # (where # is number of jobs)\n" &&
	printf "\n"
}
case $1 in
	--help) displayHelp; exit 0;;
esac

printf "\n" &&
printf "${YEL}Building Thorium .deb package...\n" &&
printf "${CYA}\n" &&

# Build debian package
export NINJA_SUMMARIZE_BUILD=1 &&

./depot_tools/autoninja -C ~/chromium/src/out/thorium "chrome/installer/linux:stable_deb" -j$@ &&

printf "${GRE}Done! ${YEL}Installer at \'//out/thorium/thorium*.deb\'\n" &&
tput sgr0
