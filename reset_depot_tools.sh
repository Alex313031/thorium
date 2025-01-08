#!/bin/bash

# Copyright (c) 2024 Alex313031 and midzer

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
	printf "${bold}${GRE}Script to reset depot_tools on Linux/MacOS.${c0}\n" &&
	printf "\n" &&
	printf "${bold}${YEL}This will remove ${CYA}depot_tools${YEL}, ${CYA}.gsutil${YEL}, ${CYA}.vpython_cipd_cache${YEL}, and ${CYA}.vpython-root${c0}\n" &&
	printf "${bold}${YEL}from your home directory, and then re-clone depot_tools in \$HOME.${c0}\n" &&
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

printf "\n" &&
printf "${YEL}Removing depot_tools, etc...${c0}\n" &&
tput sgr0 &&
printf "\n" &&

sudo rm -r -f $HOME/depot_tools &&
printf "removed \'$HOME/depot_tools\'${c0}\n" &&

sudo rm -r -f $HOME/.gsutil &&
printf "removed \'$HOME/.gsutil\'${c0}\n" &&

sudo rm -r -f $HOME/.vpython_cipd_cache &&
printf "removed \'$HOME/.vpython_cipd_cache\'${c0}\n" &&

sudo rm -r -f $HOME/.vpython-root &&
printf "removed \'$HOME/.vpython-root\'${c0}\n" &&

sudo rm -r -f $HOME/.cache/.vpython-root &&
printf "removed \'$HOME/.cache/.vpython-root\'${c0}\n" &&

printf "\n" &&
printf "${GRE}Running \`git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git\`...\n" &&
tput sgr0 &&
printf "\n" &&

cd $HOME &&

git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git &&
cd depot_tools &&
git checkout -f 31e21628c3a558d4f2189712e25849d608b3ff8c &&
cd .. &&

source ~/.bashrc &&

printf "\n" &&

printf "${GRE}Completed. ${YEL}You can now run \'./trunk.sh\'\n"
tput sgr0
