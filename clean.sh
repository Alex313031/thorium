#!/bin/bash

# Copyright (c) 2024 Alex313031.

# Copy and run from within out/thorium or wherever you put your build, or cd there first!
# i.e. cd /home/alex/bin/thorium/

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
	printf "${bold}${GRE}Script to remove unneeded artifacts in Thorium's build directory.${c0}\n" &&
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
printf "${YEL}Cleaning up build artifacts...\n" &&
printf "${RED}\n" &&

cd ${CR_SRC_DIR}/out/thorium &&

rm -r -f -v pyproto &&
rm -r -f -v obj &&
rm -r -f -v newlib_pnacl_nonsfi &&
rm -r -f -v newlib_pnacl &&
rm -r -f -v nacl_bootstrap_x64 &&
rm -r -f -v irt_x64 &&
rm -r -f -v glibc_x64 &&
rm -r -f -v gen &&
rm -r -f -v etc &&
rm -r -f -v clang_newlib_x64 &&
rm -r -f -v thinlto-cache &&
rm -r -f -v fontconfig_caches &&
find ${CR_SRC_DIR}/out/thorium -name "*deps*" -delete &&
find ${CR_SRC_DIR}/out/thorium -name "*TOC*" -delete &&

printf "${GRE}Done cleaning artifacts.\n" &&
tput sgr0
