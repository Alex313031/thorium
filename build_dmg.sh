#!/bin/bash

# Copyright (c) 2022 Alex313031 and Midzer.

YEL='\033[1;33m' # Yellow
GRE='\033[1;32m' # Green

# Error handling
yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "Failed $*"; }

printf "\n" &&
printf "${YEL}Building .dmg of Thorium...\n" &&
printf "${GRE}\n" &&

# Fix file attr
xattr -csr out/thorium/Thorium.app &&

# Sign binary
codesign --force --deep --sign - out/thorium/Thorium.app &&

# Build dmg package
chrome/installer/mac/pkg-dmg \
  --sourcefile --source out/thorium/Thorium.app \
  --target "out/thorium/Thorium_MacOS.dmg" \
  --volname Thorium --symlink /Applications:/Applications \
  --format UDBZ --verbosity 2 &&
  
  printf "${YEL}.DMG Build Completed. Installer at //out/thorium/thorium*.dmg\n" &&
  tput sgr0 &&
