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
	printf "${bold}${GRE}Script to Rebase/Sync Chromium repo to Tip of Tree.${c0}\n" &&
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
printf "${bold}${GRE}Script to Rebase/Sync Chromium repo to Tip of Tree.${c0}\n" &&
printf "\n" &&
printf "${YEL}Rebasing/Syncing to \`origin/main\` and running hooks...\n" &&
tput sgr0 &&

cd ${CR_SRC_DIR}/v8/ &&

git restore . && git clean -ffd &&

cd ${CR_SRC_DIR}/third_party/devtools-frontend/src &&

git restore . && git clean -ffd &&

cd ${CR_SRC_DIR}/third_party/ffmpeg &&

git restore . && git clean -ffd &&

cd ${CR_SRC_DIR} &&

rm -v -r -f ${CR_SRC_DIR}/third_party/pak &&

git checkout -f origin/main &&

git clean -ffd &&
git clean -ffd &&

git rebase-update &&

git fetch --tags &&

gclient sync --with_branch_heads --with_tags -f -R -D &&

git clean -ffd &&

gclient runhooks &&

printf "\n" &&
printf "${GRE}Done!" &&
printf "\n" &&
tput sgr0
