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
	printf "${bold}${GRE}Script to patch Chromium's FFMPEG for AC3/EAC3 support.${c0}\n" &&
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
printf "${bold}${GRE}Script to patch Chromium's FFMPEG for AC3/EAC3 support.${c0}\n" &&

printf "\n" &&
printf "${YEL}Patching FFMPEG for AC3 & E-AC3...${c0}\n" &&

cp -v other/ffmpeg_hevc_ac3.patch ${CR_SRC_DIR}/third_party/ffmpeg/ &&
cd ${CR_SRC_DIR}/third_party/ffmpeg &&
git apply --reject ./ffmpeg_hevc_ac3.patch &&

tput sgr0
