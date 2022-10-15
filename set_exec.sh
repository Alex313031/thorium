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

printf "\n" &&
printf "${GRE}Script to set chmod +x on all other Thorium scripts.\n" &&
tput sgr0 &&
printf "${BOLD}\n" &&

# Set executable permissions
export NINJA_SUMMARIZE_BUILD=1 &&

sudo chmod -v +x depot_tools/autoninja &&

sudo chmod -v +x build.sh &&

sudo chmod -v +x build_dmg.sh &&

sudo chmod -v +x build_mac.sh &&

sudo chmod -v +x build_win.sh &&

sudo chmod -v +x check_avx.sh &&

sudo chmod -v +x clean.sh &&

sudo chmod -v +x setup.sh &&

sudo chmod -v +x thordeb.sh &&

sudo chmod -v +x trunk.sh &&

sudo chmod -v +x reset_depot_tools.sh &&

sudo chmod -v +x infra/install_deps.sh &&

sudo chmod -v +x infra/build_polly.sh &&

sudo chmod -v +x infra/DEBUG/build_debug_linux.sh &&

sudo chmod -v +x infra/DEBUG/build_debug_shell_linux.sh &&

sudo chmod -v +x infra/DEBUG/build_debug_shell_win.sh &&

sudo chmod -v +x infra/DEBUG/build_debug_win.sh &&

sudo chmod -v +x infra/DEBUG/Thorium_Debug_Shell.sh &&

sudo chmod -v +x logos/NEW/mac/gen/app/build_icns.sh &&

sudo chmod -v +x logos/NEW/mac/gen/document/build_icns.sh &&

sudo chmod -v +x portable/THORIUM-PORTABLE &&

sudo chmod -v +x portable/THORIUM-SHELL &&

sudo chmod -v +x portable/thorium-portable.desktop &&

sudo chmod -v +x portable/thorium-shell.desktop &&

sudo chmod -v +x portable/make_portable_linux.sh &&

sudo chmod -v +x portable/make_portable_win.sh &&

sudo chmod -v +x infra/APPIMAGE/pkg2appimage &&

sudo chmod -v +x infra/APPIMAGE/build_appimage.sh &&

sudo chmod -v +x pak_src/build.sh &&

sudo chmod -v +x src/build/vs_toolchain.py &&

sudo chmod -v +x src/tools/gn/bootstrap/bootstrap.py &&

sudo chmod -v +x src/tools/clang/scripts/build.py &&

sudo chmod -v +x other/Polly/setup_polly.sh &&

printf "${GRE}${bold}Scripts are ready!\n" &&
tput sgr0
