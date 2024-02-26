#!/bin/bash

# Copyright (c) 2024 Alex313031.

YEL='\033[1;33m' # Yellow
CYA='\033[1;96m' # Cyan
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
	printf "${bold}${YEL}Script to build Thorium DEBUG on Linux.${c0}\n" &&
	printf "${underline}Usage: ${c0}build_debug_linux.sh # (where # is number of jobs)\n" &&
	printf "\n"
}
case $1 in
	--help) displayHelp; exit 0;;
esac

printf "\n" &&
printf "${YEL}Building Thorium DEBUG for Linux...\n" &&
printf "${CYA}\n" &&

# chromium/src dir env variable
if [ -z "${CR_DIR}" ]; then 
    CR_SRC_DIR="$HOME/chromium/src"
    export CR_SRC_DIR
else 
    CR_SRC_DIR="${CR_DIR}"
    export CR_SRC_DIR
fi

# Build Thorium and Thorium UI Debug Shell
export NINJA_SUMMARIZE_BUILD=1 &&

cd ${CR_SRC_DIR} &&
autoninja -C /out/thorium chrome chrome_sandbox chromedriver thorium_shell thorium_ui_debug_shell clear_key_cdm -j$@ &&
cd ~/thorium/infra/DEBUG &&

mkdir -v -p ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell &&
mkdir -v -p ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/lib &&
mkdir -v -p ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/icons &&

cp -r -f -v ./icons/icon_16.png ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/icons &&
cp -r -f -v ./icons/icon_24.png ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/icons &&
cp -r -f -v ./icons/icon_32.png ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/icons &&
cp -r -f -v ./icons/icon_48.png ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/icons &&
cp -r -f -v ./icons/icon_64.png ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/icons &&
cp -r -f -v ./icons/icon_128.png ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/icons &&
cp -r -f -v ./icons/icon_256.png ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/icons &&
cp -r -f -v ./icons/icon_256.png ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
# cp -r -f -v ./icons/thorium_debug_shell.ico ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell &&
cp -r -f -v DEBUG_SHELL_README.md ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/README.md &&
cp -r -f -v Thorium_Debug_Shell.sh ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell &&
cp -r -f -v ~/chromium/src/out/thorium/locales ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/test_fonts ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/resources ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/ui ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/libffmpeg.so ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/libffmpeg.so ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/lib &&
# cp -r -f -v ~/chromium/src/out/thorium/libblink_test_plugin.so ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/icudtl.dat ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/content_resources.pak ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/libEGL.so ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/libGLESv2.so ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/libvk_swiftshader.so ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/libvulkan.so.1 ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/vk_swiftshader_icd.json ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/v8_context_snapshot.bin ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/v8_context_snapshot_generator ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/ui_resources_100_percent.pak ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/ui_test.pak ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/ui_test_200_percent.pak ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/views_examples_resources.pak ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ~/chromium/src/out/thorium/thorium_ui_debug_shell ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&
cp -r -f -v ClearKeyCdm ~/chromium/src/out/thorium/Thorium_UI_Debug_Shell/ &&

printf "${GRE}Debug Linux Build Completed.\n" &&
tput sgr0
