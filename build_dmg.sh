#!/bin/bash

YEL='\033[1;33m' # Yellow

printf "\n" &&
printf "${YEL}Building .dmg of Thorium...\n" &&
printf "\n" &&
tput sgr0 &&

xattr -csr out/thorium/Thorium.app &&

codesign --force --deep --sign - out/thorium/Thorium.app &&

chrome/installer/mac/pkg-dmg \
  --sourcefile --source out/thorium/Thorium.app \
  --target "out/thorium/Thorium_MacOS.dmg" \
  --volname Thorium --symlink /Applications:/Applications \
  --format UDBZ --verbosity 2
