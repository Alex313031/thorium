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

# help
	printf "\n" &&
	printf "${bold}${GRE}Script to copy Thorium source files over the Chromium source tree.${c0}\n" &&
	printf "${bold}${YEL}Use the --regular flag for Regular (AVX) builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --avx2 flag for AVX2 Builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --sse3 flag for SSE3 Builds.${c0}\n" &&
	printf "${bold}${YEL}IMPORTANT: For Polly builds, first run build_polly.sh in Thorium/infra, then use this script.${c0}\n" &&
	printf "${bold}${YEL}Both of these actions should be taken AFTER running the regular setup.sh script!${c0}\n" &&
	printf "\n" &&

tput sgr0 &&

# Copy Polly AVX files
copyAVX () {
	printf "\n" &&
	printf "${YEL}Copying Polly AVX build files...${c0}\n" &&
	cp -r -v Regular/build.gn $HOME/chromium/src/build/config/compiler/ &&
	printf "\n"
}

case $1 in
	--avx) copyAVX;
esac

# Copy Polly AVX2 files
copyAVX2 () {
	printf "\n" &&
	printf "${YEL}Copying Polly AVX2 build files...${c0}\n" &&
	cp -r -v AVX2/build.gn $HOME/chromium/src/build/config/compiler/ &&
	printf "\n"
}

case $1 in
	--avx2) copyAVX2;
esac

# Copy Polly SSE3 files
copySSE3 () {
	printf "\n" &&
	printf "${YEL}Copying Polly SSE3 build files...${c0}\n" &&
	cp -r -v SSE3/build.gn $HOME/chromium/src/build/config/compiler/ &&
	printf "\n"
}

case $1 in
	--sse3) copySSE3;
esac

printf "${GRE}Done!\n" &&
printf "\n" &&

printf "${YEL}Exporting variables...\n" &&

export NINJA_SUMMARIZE_BUILD=1 &&

export EDITOR=nano &&

export VISUAL=nano &&
printf "\n" &&
tput sgr0 &&

printf "export ${CYA}NINJA_SUMMARIZE_BUILD=1${c0}\n" &&

printf "export ${CYA}EDITOR=nano${c0}\n" &&

printf "export ${CYA}VISUAL=nano${c0}\n" &&
printf "\n" &&

printf "${GRE}Enjoy Thorium!\n" &&
printf "\n" &&
tput sgr0

exit 0
