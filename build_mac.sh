#!/bin/bash

# Copyright (c) 2022 Alex313031.

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
	printf "${bold}${GRE}Script to build Thorium and Thorium Shell on MacOS.${c0}\n" &&
	printf "${underline}${YEL}Usage: ${c0}build.sh # (where # is number of jobs)${c0}\n" &&
	printf "${c0}Use the --build-shell flag to also build the thorium_shell target.${c0}\n" &&
	printf "\n"
}

# Build Thorium Shell in addition to the others.
buildShell () {
	printf "\n" &&
	printf "${YEL}Building Thorium and Thorium Shell for MacOS...\n" &&
	printf "${CYA}\n" &&
	
	# Build Thorium
	export NINJA_SUMMARIZE_BUILD=1 &&
	
	./depot_tools/autoninja -C ~/chromium/src/out/thorium chrome chromedriver thorium_shell -j$@ &&
	
	cat logos/thorium_logo_ascii_art.txt &&
	
	printf "${GRE}${bold}Build Completed. ${YEL}${bold}You can now run ./build_dmg.sh, and copy the Thorium Shell.app\n" &&
	tput sgr0
	}
	
case $1 in
	--help) displayHelp; exit 0;;
esac

case $1 in
	--build-shell) buildShell; exit 0;;
esac

printf "\n" &&
printf "${YEL}Building Thorium for MacOS...\n" &&
printf "${CYA}\n" &&

# Build Thorium
export NINJA_SUMMARIZE_BUILD=1 &&

./depot_tools/autoninja -C ~/chromium/src/out/thorium chrome chromedriver thorium_shell -j$@ &&

cat logos/thorium_logo_ascii_art.txt &&

printf "${GRE}${bold}Build Completed. ${YEL}${bold}You can now run ./build_dmg.sh\n" &&

exit 0
