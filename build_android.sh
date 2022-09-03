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

# --help
displayHelp () {
	printf "\n" &&
	printf "${bold}${GRE}Script to build Thorium for Android.${c0}\n" &&
	printf "${underline}${YEL}Usage: ${c0}build.sh # (where # is number of jobs)${c0}\n" &&
	printf "\n"
}

case $1 in
	--help) displayHelp; exit 0;;
esac

printf "\n" &&
printf "${YEL}Building Thorium for Android...\n" &&
printf "${YEL}Note: You may want ADB installed.${c0}\n" &&
printf "${CYA}\n" &&

# Workaround for branding dir
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_base/drawable-v26/ic_launcher.xml &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_base/drawable-v26/ic_launcher_round.xml &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_base/values/ic_launcher_round_alias.xml &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-mdpi/layered_app_icon_background.png &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-mdpi/layered_app_icon.png &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-xhdpi/layered_app_icon_background.png &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-xhdpi/layered_app_icon.png &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-xxxhdpi/layered_app_icon_background.png &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-xxxhdpi/layered_app_icon.png &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-nodpi/layered_app_icon_foreground.xml &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-hdpi/layered_app_icon_background.png &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-hdpi/layered_app_icon.png &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-xxhdpi/layered_app_icon_background.png &&
rm -v -r -f $HOME/chromium/src/chrome/android/java/res_chromium_base/mipmap-xxhdpi/layered_app_icon.png &&

# Build Thorium
export NINJA_SUMMARIZE_BUILD=1 &&

./depot_tools/autoninja -C ~/chromium/src/out/thorium chrome_public_apk system_webview_apk -j$@ &&

cat $HOME/thorium/logos/thorium_logo_ascii_art.txt &&

printf "${GRE}${bold}Build Completed. ${YEL}${bold}You can copy the .apks to your device or use ADB to install.\n" &&
tput sgr0 &&

exit 0
