#!/bin/bash

# Copyright (c) 2024 Alex313031.

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
	printf "${bold}${GRE}Script to build libffmpeg.so or ffmpeg.dll.${c0}\n" &&
	printf "${underline}${YEL}Usage:${c0} build.sh # (where # is number of jobs)${c0}\n" &&
	printf "\n"
}
case $1 in
	--help) displayHelp; exit 0;;
esac

# chromium/src dir env variable
if [ -z "${CR_DIR}" ]; then 
    CR_SRC_DIR="$HOME/chromium/src"
    export CR_SRC_DIR
else 
    CR_SRC_DIR="${CR_DIR}"
    export CR_SRC_DIR
fi

printf "\n" &&
printf "${YEL}Building libffmpeg...\n" &&
printf "${CYA}\n" &&

# Build Thorium
export NINJA_SUMMARIZE_BUILD=1 &&
export NINJA_STATUS="[%r processes, %f/%t @ %o/s | %e sec. ] " &&

cd ${CR_SRC_DIR} &&
autoninja -C out/thorium/ third_party/ffmpeg:ffmpeg third_party/ffmpeg:ffmpeg_features third_party/ffmpeg:ffmpeg_internal -j$@ &&

printf "${GRE}${bold}Done!\n" &&
tput sgr0
