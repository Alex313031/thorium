#!/bin/bash

# Copyright (c) 2022 Alex313031.

YEL='\033[1;33m' # Yellow
c0=$'\033[0m' # Reset Text
bold=$'\033[1m' # Bold Text
underline=$'\033[4m' # Underline Text

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
printf "${YEL}Building Thorium...\n" &&
printf "\n" &&
tput sgr0 &&

export NINJA_SUMMARIZE_BUILD=1 &&

./infra/autoninja -C ~/chromium/src/out/thorium chrome chromedriver content_shell setup mini_installer -j$@
