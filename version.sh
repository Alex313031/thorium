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
	printf "${bold}${YEL}Script to check out Chromium tag of current Thorium version.${c0}\n" &&
	printf "\n"
	printf "${RED}NOTE: You may need to run ${c0}${bold}./trunk.sh ${RED}before using this script!${c0}\n" &&
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

THOR_VER="121.0.6167.206"

export THOR_VER &&

printf "\n"
printf "${GRE}Current Thorium version is:${c0} ${underline}$THOR_VER${c0}\n"
printf "\n"
printf "${RED}NOTE: ${YEL}Checking out${CYA} tags/$THOR_VER ${YEL}in ${CR_SRC_DIR}...${c0}\n"
printf "\n"

cd ${CR_SRC_DIR} &&

git checkout -f tags/$THOR_VER &&

cd ~/thorium &&

# Use our artifacts hash
cp -v src/build/vs_toolchain.py ${CR_SRC_DIR}/build/ &&

# Add //third_party/libjxl to DEPS
cp -v thorium-libjxl/src/DEPS ${CR_SRC_DIR}/ &&

cd ${CR_SRC_DIR} &&

git clean -ffd &&

gclient sync --with_branch_heads --with_tags -f -R -D &&

gclient runhooks &&

# Install all sysroots (i.e. for ARM64)
build/linux/sysroot_scripts/install-sysroot.py --all &&

printf "\n"
printf "${GRE}Chromium tree is checked out at tag: ${c0}$THOR_VER\n"
printf "\n"
	
printf "${YEL}Downloading PGO Profiles for Linux, Windows, and Mac...\n" &&
printf "\n" &&
tput sgr0 &&

python3 tools/update_pgo_profiles.py --target=linux update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&

python3 tools/update_pgo_profiles.py --target=win64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&

python3 tools/update_pgo_profiles.py --target=mac update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&
printf "\n" &&

printf "${YEL}Downloading PGO Profile for V8 (when v8_enable_builtins_optimization = true)\n" &&
printf "\n" &&
tput sgr0 &&

python3 v8/tools/builtins-pgo/download_profiles.py --depot-tools=$HOME/depot_tools download &&
printf "\n" &&

cd ~/thorium &&

printf "${GRE}Done! ${YEL}You can now run \'./setup.sh\'\n"
tput sgr0
