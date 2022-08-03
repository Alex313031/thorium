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
	
	printf "${GRE}${bold}Build Completed. ${YEL}${bold}You can now run ./build_dmg.sh, and copy the Thorium Shell.app\n" &&
	tput sgr0 &&
	
	c0='\033[0m' # Reset Text
	c1='\033[0m\033[36m\033[1m' # Light Cyan
	c2='\033[0m\033[1;31m' # Light Red
	c3='\033[0m\033[37m' # Light Grey
	c4='\033[0m\033[1;34m\033[1m' # Light Blue
	c5='\033[0m\033[1;37m' # White
	c6='\033[0m\033[1;34m' # Dark Blue
	c7='\033[1;32m' # Green
	
	printf "\n" &&
	printf "${c4}                .,:loool:,.              \n" &&
	printf "${c4}            .,coooooooooooooc,.          \n" &&
	printf "${c4}         .,lllllllllllllllllllll,.       \n" &&
	printf "${c4}        ;ccccccccccccccccccccccccc;      \n" &&
	printf "${c1}      ,${c4}ccccccccccccccccccccccccccccc.    \n" &&
	printf "${c1}     ,oo${c4}c::::::::ok${c5}00000${c3}OOkkkkkkkkkkk:   \n" &&
	printf "${c1}    .ooool${c4};;;;:x${c5}K0${c6}kxxxxxk${c5}0X${c3}K0000000000.  \n" &&
	printf "${c1}    :oooool${c4};,;O${c5}K${c6}ddddddddddd${c5}KX${c3}000000000d  \n" &&
	printf "${c1}    lllllool${c4};l${c5}N${c6}dllllllllllld${c5}N${c3}K000000000  \n" &&
	printf "${c1}    lllllllll${c4}o${c5}M${c6}dccccccccccco${c5}W${c3}K000000000  \n" &&
	printf "${c1}    ;cllllllllX${c5}X${c6}c:::::::::c${c5}0X${c3}000000000d  \n" &&
	printf "${c1}    .ccccllllllO${c5}Nk${c6}c;,,,;cx${c5}KK${c3}0000000000.  \n" &&
	printf "${c1}     .cccccclllllxO${c5}OOOO0${c1}kx${c3}O0000000000;   \n" &&
	printf "${c1}      .:ccccccccllllllllo${c3}O0000000OOO,    \n" &&
	printf "${c1}        ,:ccccccccclllcd${c3}0000OOOOOOl.     \n" &&
	printf "${c1}          .::ccccccccc${c3}dOOOOOOOkx:.       \n" &&
	printf "${c1}            ..,::cccc${c3}xOOOkkko;.          \n" &&
	printf "${c1}               ..::${c3}dOkkxl:.              \n" &&
	printf "\n"
	printf "${c7}            Long Live Chromium\041\n${c0}\n"
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

printf "${GRE}${bold}Build Completed. ${YEL}${bold}You can now run ./build_dmg.sh\n" &&
tput sgr0 &&

c0='\033[0m' # Reset Text
c1='\033[0m\033[36m\033[1m' # Light Cyan
c2='\033[0m\033[1;31m' # Light Red
c3='\033[0m\033[37m' # Light Grey
c4='\033[0m\033[1;34m\033[1m' # Light Blue
c5='\033[0m\033[1;37m' # White
c6='\033[0m\033[1;34m' # Dark Blue
c7='\033[1;32m' # Green

printf "\n" &&
printf "${c4}                .,:loool:,.              \n" &&
printf "${c4}            .,coooooooooooooc,.          \n" &&
printf "${c4}         .,lllllllllllllllllllll,.       \n" &&
printf "${c4}        ;ccccccccccccccccccccccccc;      \n" &&
printf "${c1}      ,${c4}ccccccccccccccccccccccccccccc.    \n" &&
printf "${c1}     ,oo${c4}c::::::::ok${c5}00000${c3}OOkkkkkkkkkkk:   \n" &&
printf "${c1}    .ooool${c4};;;;:x${c5}K0${c6}kxxxxxk${c5}0X${c3}K0000000000.  \n" &&
printf "${c1}    :oooool${c4};,;O${c5}K${c6}ddddddddddd${c5}KX${c3}000000000d  \n" &&
printf "${c1}    lllllool${c4};l${c5}N${c6}dllllllllllld${c5}N${c3}K000000000  \n" &&
printf "${c1}    lllllllll${c4}o${c5}M${c6}dccccccccccco${c5}W${c3}K000000000  \n" &&
printf "${c1}    ;cllllllllX${c5}X${c6}c:::::::::c${c5}0X${c3}000000000d  \n" &&
printf "${c1}    .ccccllllllO${c5}Nk${c6}c;,,,;cx${c5}KK${c3}0000000000.  \n" &&
printf "${c1}     .cccccclllllxO${c5}OOOO0${c1}kx${c3}O0000000000;   \n" &&
printf "${c1}      .:ccccccccllllllllo${c3}O0000000OOO,    \n" &&
printf "${c1}        ,:ccccccccclllcd${c3}0000OOOOOOl.     \n" &&
printf "${c1}          .::ccccccccc${c3}dOOOOOOOkx:.       \n" &&
printf "${c1}            ..,::cccc${c3}xOOOkkko;.          \n" &&
printf "${c1}               ..::${c3}dOkkxl:.              \n" &&
printf "\n"
printf "${c7}            Long Live Chromium\041\n${c0}\n"
