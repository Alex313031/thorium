#!/bin/bash

# Copyright (c) 2024 Alex313031, midzer, and gz83.

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
	printf "${bold}${GRE}Script to Rebase/Sync the Chromium repo.${c0}\n" &&
	# printf "${bold}${YEL}Use the --shallow flag to do a shallow sync, if you have downloaded${c0}\n" &&
	# printf "${bold}${YEL}the Chromium repo with the --no-history flag.${c0}\n" &&
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
printf "${bold}${GRE}Script to Rebase/Sync the Chromium repo.${c0}\n" &&
printf "\n" &&
printf "${YEL}Rebasing/Syncing and running hooks...\n" &&
tput sgr0 &&

cd ${CR_SRC_DIR}/v8/ &&

git restore . && git clean -ffd &&

cd ${CR_SRC_DIR}/third_party/devtools-frontend/src &&

git restore . && git clean -ffd &&

cd ${CR_SRC_DIR}/third_party/ffmpeg &&

git restore . && git clean -ffd &&

cd ${CR_SRC_DIR} &&

rm -v -r -f ${CR_SRC_DIR}/third_party/pak &&

rm -r -f -v	components/ungoogled/ &&

git checkout -f origin/main &&

git clean -ffd &&
git clean -ffd &&

git rebase-update &&

git fetch --tags &&

# Use our artifacts hash
cd $HOME/thorium &&
cp -v src/build/vs_toolchain.py ${CR_SRC_DIR}/build/ &&
cd ${CR_SRC_DIR} &&

gclient sync --with_branch_heads --with_tags -f -R -D &&

git clean -ffd &&

gclient runhooks &&

printf "\n" &&
printf "${GRE}Done! ${YEL}You can now run \'./version.sh\'\n" &&
tput sgr0 &&

#c0='\033[0m' # Reset Text
#c1='\033[0m\033[36m\033[1m' # Light Cyan
#c2='\033[0m\033[1;31m' # Light Red
#c3='\033[0m\033[37m' # Light Grey
#c4='\033[0m\033[1;34m\033[1m' # Light Blue
#c5='\033[0m\033[1;37m' # White
#c6='\033[0m\033[1;34m' # Dark Blue
#c7='\033[1;32m' # Green

#printf "\n" &&
#printf "${c4}                .,:loool:,.              \n" &&
#printf "${c4}            .,coooooooooooooc,.          \n" &&
#printf "${c4}         .,lllllllllllllllllllll,.       \n" &&
#printf "${c4}        ;ccccccccccccccccccccccccc;      \n" &&
#printf "${c1}      ,${c4}ccccccccccccccccccccccccccccc.    \n" &&
#printf "${c1}     ,oo${c4}c::::::::ok${c5}00000${c3}OOkkkkkkkkkkk:   \n" &&
#printf "${c1}    .ooool${c4};;;;:x${c5}K0${c6}kxxxxxk${c5}0X${c3}K0000000000.  \n" &&
#printf "${c1}    :oooool${c4};,;O${c5}K${c6}ddddddddddd${c5}KX${c3}000000000d  \n" &&
#printf "${c1}    lllllool${c4};l${c5}N${c6}dllllllllllld${c5}N${c3}K000000000  \n" &&
#printf "${c1}    lllllllll${c4}o${c5}M${c6}dccccccccccco${c5}W${c3}K000000000  \n" &&
#printf "${c1}    ;cllllllllX${c5}X${c6}c:::::::::c${c5}0X${c3}000000000d  \n" &&
#printf "${c1}    .ccccllllllO${c5}Nk${c6}c;,,,;cx${c5}KK${c3}0000000000.  \n" &&
#printf "${c1}     .cccccclllllxO${c5}OOOO0${c1}kx${c3}O0000000000;   \n" &&
#printf "${c1}      .:ccccccccllllllllo${c3}O0000000OOO,    \n" &&
#printf "${c1}        ,:ccccccccclllcd${c3}0000OOOOOOl.     \n" &&
#printf "${c1}          .::ccccccccc${c3}dOOOOOOOkx:.       \n" &&
#printf "${c1}            ..,::cccc${c3}xOOOkkko;.          \n" &&
#printf "${c1}               ..::${c3}dOkkxl:.              \n" &&
#printf "\n"
#printf "${c7}            Long Live Chromium\041\n${c0}\n" &&

printf "\n" &&
cat ~/thorium/logos/chromium_logo_ascii_art.txt &&
printf "\n" &&
tput sgr0
