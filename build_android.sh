#!/bin/bash

# Copyright (c) 2024 Alex313031.

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
	printf "${bold}${GRE}Script to build Thorium for Android.${c0}\n" &&
	printf "${underline}${YEL}Usage:${c0}${bold} build_android.sh --arm32 | --arm64 | --x86 | --x64 # (where # is number of jobs)${c0}\n" &&
	printf " Use the --help flag to show this help.${c0}\n" &&
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

cr_build_jobs="$2"
export cr_build_jobs

printf "\n" &&
printf "${bold}${GRE}Script to build Thorium for Android.${c0}\n" &&
printf "${underline}${YEL}Usage:${c0}${bold} build_android.sh --arm32 | --arm64 | --x86 | --x64 # (where # is number of jobs)${c0}\n" &&
printf " Use the --help flag to show this help.${c0}\n" &&

buildARM32 () {
	printf "\n" &&
	printf "${YEL}Building Thorium for Android arm32...\n" &&
	printf "${YEL}Note: You may want ADB installed.${c0}\n" &&
	printf "${CYA}\n" &&

	# Build Thorium, Thorium Shell, and System WebView32
	export NINJA_SUMMARIZE_BUILD=1 &&
	export NINJA_STATUS="[%r processes, %f/%t @ %o/s | %e sec. ] " &&

	cd ${CR_SRC_DIR} &&
	autoninja -C out/thorium chrome_public_apk content_shell_apk system_webview_apk -j${cr_build_jobs} &&
	printf "\n" &&
	cat ~/thorium/logos/thorium_logo_ascii_art.txt &&
	printf "\n" &&
	printf "${GRE}${bold}Build Completed. ${YEL}${bold}You can copy the .apk(s) to your device or use ADB to install it.\n" &&
	printf "${GRE}${bold}They are located in \'//chromium/src/out/thorium/apks/\'\n" &&
	printf "\n"
}
case $1 in
	--arm32) buildARM32;
esac

buildARM64 () {
	printf "\n" &&
	printf "${YEL}Building Thorium for Android arm64...\n" &&
	printf "${YEL}Note: You may want ADB installed.${c0}\n" &&
	printf "${CYA}\n" &&

	# Build Thorium, Thorium Shell, and System WebView64
	export NINJA_SUMMARIZE_BUILD=1 &&
	export NINJA_STATUS="[%r processes, %f/%t @ %o/s | %e sec. ] " &&

	cd ${CR_SRC_DIR} &&
	autoninja -C out/thorium chrome_public_apk content_shell_apk system_webview_64_apk -j${cr_build_jobs} &&
	printf "\n" &&
	cat ~/thorium/logos/thorium_logo_ascii_art.txt &&
	printf "\n" &&
	printf "${GRE}${bold}Build Completed. ${YEL}${bold}You can copy the .apk(s) to your device or use ADB to install it.\n" &&
	printf "${GRE}${bold}They are located in \'//chromium/src/out/thorium/apks/\'\n" &&
	printf "\n"
}
case $1 in
	--arm64) buildARM64;
esac

buildX86 () {
	printf "\n" &&
	printf "${YEL}Building Thorium for Android x86...\n" &&
	printf "${YEL}Note: You may want ADB installed.${c0}\n" &&
	printf "${CYA}\n" &&

	# Build Thorium, Thorium Shell, and System WebView x86
	export NINJA_SUMMARIZE_BUILD=1 &&
	export NINJA_STATUS="[%r processes, %f/%t @ %o/s | %e sec. ] " &&

	cd ${CR_SRC_DIR} &&
	autoninja -C out/thorium chrome_public_apk content_shell_apk system_webview_apk -j${cr_build_jobs} &&
	printf "\n" &&
	cat ~/thorium/logos/thorium_logo_ascii_art.txt &&
	printf "\n" &&
	printf "${GRE}${bold}Build Completed. ${YEL}${bold}You can copy the .apk(s) to your device or use ADB to install it.\n" &&
	printf "${GRE}${bold}They are located in \'//chromium/src/out/thorium/apks/\'\n" &&
	printf "\n"
}
case $1 in
	--x86) buildX86;
esac

buildX64 () {
	printf "\n" &&
	printf "${YEL}Building Thorium for Android x64...\n" &&
	printf "${YEL}Note: You may want ADB installed.${c0}\n" &&
	printf "${CYA}\n" &&

	# Build Thorium, Thorium Shell, and System WebView x86
	export NINJA_SUMMARIZE_BUILD=1 &&
	export NINJA_STATUS="[%r processes, %f/%t @ %o/s | %e sec. ] " &&

	cd ${CR_SRC_DIR} &&
	autoninja -C out/thorium chrome_public_apk content_shell_apk system_webview_apk -j${cr_build_jobs} &&
	printf "\n" &&
	cat ~/thorium/logos/thorium_logo_ascii_art.txt &&
	printf "\n" &&
	printf "${GRE}${bold}Build Completed. ${YEL}${bold}You can copy the .apk(s) to your device or use ADB to install it.\n" &&
	printf "${GRE}${bold}They are located in \'//chromium/src/out/thorium/apks/\'\n" &&
	printf "\n"
}
case $1 in
	--x64) buildX64;
esac

tput sgr0
