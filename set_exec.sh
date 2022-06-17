#!/bin/bash

# Copyright (c) 2022 Alex313031.

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

printf "\n" &&
printf "${YEL}Script to set chmod +x on all other Thorium scripts.\n" &&
printf "${GRE}\n" &&

# Set executable permissions
export NINJA_SUMMARIZE_BUILD=1 &&

sudo chmod -v +x build.sh &&

sudo chmod -v +x build_dmg.sh &&

sudo chmod -v +x build_mac.sh &&

sudo chmod -v +x build_win.sh &&

sudo chmod -v +x check_avx.sh &&

sudo chmod -v +x clean.sh &&

sudo chmod -v +x setup.sh &&

sudo chmod -v +x setup_mac.sh &&

sudo chmod -v +x thordeb.sh &&

sudo chmod -v +x trunk.sh &&

sudo chmod -v +x infra/build_polly.sh &&

sudo chmod -v +x infra/DEBUG/build_debug_linux.sh &&

sudo chmod -v +x infra/DEBUG/build_debug_shell.sh &&

sudo chmod -v +x infra/DEBUG/build_debug_win.sh &&

sudo chmod -v +x infra/DEBUG/Thorium_Debug_Shell.sh &&

sudo chmod -v +x logos/NEW/mac/gen/app/build_icns.sh &&

sudo chmod -v +x logos/NEW/mac/gen/document/build_icns.sh &&

sudo chmod -v +x portable/thorium-browser &&

sudo chmod -v +x portable/thorium-shell &&

sudo chmod -v +x portable/thorium-browser.desktop &&

sudo chmod -v +x portable/thorium-shell.desktop &&

sudo chmod -v +x infra/APPIMAGE/pkg2appimage &&

sudo chmod -v +x infra/APPIMAGE/build_appimage.sh &&

printf "${GRE}${bold}Scripts are ready!\n" &&
tput sgr0
