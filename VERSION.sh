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
	printf "${bold}${YEL}Script to check out Chromium tag of current Thorium version.${c0}\n" &&
	printf "\n"
	printf "${RED}NOTE: You may need to run ${c0}${bold}./trunk.sh ${RED}before using this script!${c0}\n" &&
	printf "\n"
}

case $1 in
	--help) displayHelp; exit 0;;
esac

THOR_VER="109.0.5414.131"

export THOR_VER &&

printf "\n"
printf "${GRE}Current Thorium version is:${c0} ${underline}$THOR_VER${c0}\n"
printf "\n"
printf "${RED}NOTE: ${YEL}Checking out${CYA} tags/$THOR_VER ${YEL}in $HOME/chromium/src...${c0}\n"
printf "\n"

cd ~/chromium/src &&

git checkout -f tags/$THOR_VER &&

git clean -ffd &&

gclient sync --with_branch_heads --with_tags -f -R -D &&

gclient runhooks &&

cd ~/thorium &&

printf "\n"
printf "${GRE}Chromium tree is checked out at tag: ${c0}$THOR_VER\n"
printf "\n"

exit 0
