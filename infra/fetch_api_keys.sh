#!/bin/bash

# Copyright (c) 2024 Alex313031.

# Simply uses wget to fetch Thorium API Keys.
# This is to keep them out of the main repo, in the case Google
# wants to crack down on distributing them.

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
printf "${YEL}Downloading API_KEYS.txt here...${c0}\n" &&
printf "\n" &&

rm -f ./API_KEYS.txt &&
rm -f ../API_KEYS.txt &&

wget https://paste.c-net.org/PillarCemetery -O ../API_KEYS.txt &&

printf "${GRE}Done.\n" &&
printf "${YEL}- Use the three lines therein at the atop of your args.gn\n" &&
printf "${YEL}  (Replacing the ones already in there that have blank values)\n" &&
tput sgr0
