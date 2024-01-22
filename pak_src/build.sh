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
	printf "${bold}${GRE}Script to build pak on Linux.${c0}\n" &&
	printf "${bold}${YEL}Use the --clean flag to delete the build directory.${c0}\n" &&
	printf "\n"
}
case $1 in
	--help) displayHelp; exit 0;;
esac

# Clean build dir.
cleanBuild () {
	printf "\n" &&
	printf "${bold}${YEL}Cleaning build directory.${c0}\n" &&
	printf "\n" &&
	sleep 1 &&
	rm -r -f -v ./build_
	printf "\n" &&
	printf "${bold}${GRE}Done.${c0}\n" &&
	printf "\n" &&
}
case $1 in
	--clean) cleanBuild; exit 0;;
esac

mkdir -v -p build_ &&

cd build_ &&

cmake -GNinja -DLGPL=OFF .. &&

ninja -v -d stats &&

cd ../
