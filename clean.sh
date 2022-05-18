#!/bin/bash

# Copyright (c) 2022 Alex313031.

# Copy and run from within out/thorium or wherever you put your build, or cd there first!
# i.e. cd /home/alex/bin/thorium/

YEL='\033[1;33m' # Yellow

printf "\n" &&
printf "${YEL}Cleaning up build artifacts...\n" &&
printf "\n" &&
tput sgr0 &&

cd ~/chromium/src/out/thorium

find . -name "*deps*" -delete
find . -name "*TOC*" -delete
rm -r pyproto
rm -r obj
rm -r newlib_pnacl_nonsfi
rm -r newlib_pnacl
rm -r nacl_bootstrap_x64
rm -r irt_x64
rm -r glibc_x64
rm -r gen
rm -r etc
rm -r clang_newlib_x64
rm -r thinlto-cache
rm -r fontconfig_caches

printf "${YEL}Done!\n" &&
tput sgr0

exit 0
