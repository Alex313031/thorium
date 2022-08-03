#!/bin/bash

# Copyright (c) 2022 Alex313031.

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

printf "\n" &&
printf "${YEL}Cleaning up build artifacts...\n" &&
printf "${RED}\n" &&

cd ~/chromium/src/out/thorium &&

find . -name "*deps*" -delete &&
find . -name "*TOC*" -delete &&
rm -r -v pyproto &&
rm -r -v obj &&
rm -r -v newlib_pnacl_nonsfi &&
rm -r -v newlib_pnacl &&
rm -r -v nacl_bootstrap_x64 &&
rm -r -v irt_x64 &&
rm -r -v glibc_x64 &&
rm -r -v gen &&
rm -r -v etc &&
rm -r -v clang_newlib_x64 &&
rm -r -v thinlto-cache &&
rm -r -v fontconfig_caches &&

printf "${GRE}Done cleaning artifacts.\n" &&
tput sgr0

exit 0
