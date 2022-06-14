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
	printf "${bold}${YEL}Script to build Thorium on Linux.${c0}\n" &&
	printf "${underline}Usage${c0}: build.sh # (where # is number of jobs)${c0}\n" &&
	printf "\n"
}

case $1 in
	--help) displayHelp; exit 0;;
esac

printf "\n" &&
printf "${YEL}Building Thorium for Linux...\n" &&
printf "${GRE}\n" &&

# Build Thorium
export NINJA_SUMMARIZE_BUILD=1 &&

./depot_tools/autoninja -C ~/chromium/src/out/thorium chrome chrome_sandbox chromedriver thorium_shell -j$@ &&

printf "${GRE}${bold}Build Completed.\n" &&
tput sgr0
