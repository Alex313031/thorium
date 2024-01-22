#!/bin/bash

## Copyright (c) 2024 Alex313031.

## Clones current LLVM being used by the Chromium Project, and builds a local LLVM toolchain with Polly to use the Polly optimizations in the main Thorium BUILD.gn

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

# chromium/src dir env variable
if [ -z "${CR_DIR}" ]; then 
    CR_SRC_DIR="$HOME/chromium/src"
    export CR_SRC_DIR
else 
    CR_SRC_DIR="${CR_DIR}"
    export CR_SRC_DIR
fi

# --help
displayHelp () {
	cd ${CR_SRC_DIR} &&
	python3 tools/clang/scripts/build.py --help &&
	printf "\n" &&
	printf "${bold}${GRE}Script to clone the latest LLVM being used by the Chromium Project, and builds a local ${c0}\n" &&
	printf "${bold}${GRE}LLVM toolchain with Polly to use the Polly optimizations in the main Thorium BUILD.gn${c0}\n" &&
	printf "${bold}${YEL}Use the --pgo flag to build LLVM with PGO.${c0}\n" &&
	printf "${bold}${YEL}Use the --jobs or -j flag to set the number of jobs (should be less or equal to the number of CPU cores).${c0}\n" &&
	printf "${bold}${YEL}Use the --verbose or -v flag to make Ninja show verbose commands.${c0}\n" &&
	printf "${bold}${YEL}Use the --version flag to show version, and --help to show help.${c0}\n" &&
	printf "\n"
}
case $1 in
	--help) displayHelp; exit 0;;
esac

# --version
displayVersion () {
	cd ${CR_SRC_DIR} &&
	printf "\n" &&
	python3 tools/clang/scripts/build.py --version &&
	printf "\n"
}
case $1 in
	--version) displayVersion; exit 0;;
esac

# Build with PGO
buildPollyPGO () {
	cd ${CR_SRC_DIR} &&

	printf "${GRE}Building LLVM and Polly using PGO...${c0}\n" &&
	printf "\n"
	sleep 1 &&
	
	python3 tools/clang/scripts/build.py --without-android --without-fuchsia --disable-asserts --bootstrap --thinlto --pgo $@ &&
	
	printf "\n"
	printf "${GRE}Done! You can now run ./build.sh\n" &&
	printf "\n" &&
	tput sgr0
}
case $1 in
	--pgo) buildPollyPGO; exit 0;;
esac

printf "\n" &&
printf "${bold}${GRE}Clones latest LLVM being used by the Chromium Project, and builds a local LLVM toolchain with Polly ${c0}\n" &&
printf "${bold}${GRE}to use the Polly optimizations in the main Thorium BUILD.gn${c0}\n" &&
printf "${bold}${YEL}Use the --pgo flag to build LLVM with PGO.${c0}\n" &&
printf "${bold}${YEL}Use the --jobs or -j flag to set the number of jobs (should be less or equal to the number of CPU cores)${c0}\n" &&
printf "${bold}${YEL}Use the --verbose or -v flag to make Ninja show verbose commands.${c0}\n" &&
printf "${bold}${YEL}Use the --version flag to show version, and --help to show help.${c0}\n" &&
printf "\n"

cd ${CR_SRC_DIR} &&

printf "${GRE}Building LLVM and Polly...${c0}\n" &&
printf "\n"
sleep 1 &&

python3 tools/clang/scripts/build.py --without-android --without-fuchsia --disable-asserts --bootstrap --thinlto $@ &&

printf "\n"
printf "${GRE}Done! You can now run ./build.sh\n" &&
printf "\n" &&
tput sgr0
