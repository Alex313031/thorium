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
	printf "${bold}${YEL}Script to build Thorium for Windows on Linux.${c0}\n" &&
	printf "${underline}Usage${c0}: build_win.sh # (where # is number of jobs)\n" &&
	printf "\n"
}

case $1 in
	--help) displayHelp; exit 0;;
esac

printf "\n" &&
printf "${YEL}Building Thorium for Windows...\n" &&
printf "${GRE}\n" &&

# Build Thorium and mini_installer
export NINJA_SUMMARIZE_BUILD=1 &&

./depot_tools/autoninja -C ~/chromium/src/out/thorium chrome chromedriver thorium_shell setup mini_installer -j$@ &&

mv -v -f ~/chromium/src/out/thorium/mini_installer.exe ~/chromium/src/out/thorium/thorium_mini_installer.exe &&

printf "${GRE}Build Completed. ${YEL}Installer at //out/thorium/thorium_mini_installer.exe\n" &&
tput sgr0
