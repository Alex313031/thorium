#!/bin/bash

# Copyright (c) 2024 Alex313031.

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
printf "${YEL}Generating Thorium App .icns file for MacOS...\n" &&
printf "${GRE}\n" &&

# Copy .pngs
export output_iconset_name="Thorium.iconset" &&

mkdir $output_iconset_name &&

cp -v ./icon_16x16.png ./Thorium.iconset/ &&
cp -v ./icon_16x16@2x.png ./Thorium.iconset/ &&
cp -v ./icon_32x32.png ./Thorium.iconset/ &&
cp -v ./icon_32x32@2x.png ./Thorium.iconset/ &&
cp -v ./icon_64x64.png ./Thorium.iconset/ &&
cp -v ./icon_64x64@2x.png ./Thorium.iconset/ &&
cp -v ./icon_128x128.png ./Thorium.iconset/ &&
cp -v ./icon_128x128@2x.png ./Thorium.iconset/ &&
cp -v ./icon_256x256.png ./Thorium.iconset/ &&
cp -v ./icon_256x256@2x.png ./Thorium.iconset/ &&
cp -v ./icon_512x512.png ./Thorium.iconset/ &&
cp -v ./icon_512x512@2x.png ./Thorium.iconset/ &&

# Generate .icns file
iconutil -c icns $output_iconset_name &&

# Remove temp dir
rm -R -v $output_iconset_name

printf "\n" &&
printf "${GRE}Done!\n" &&
tput sgr0
