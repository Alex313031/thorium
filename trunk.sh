#!/bin/bash

# Copyright (c) 2022 Alex313031 and Midzer.

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
	printf "${bold}${GRE}Script to Rebase/Sync Chromium repo on Linux.${c0}\n" &&
	printf "${bold}${YEL}Use the --shallow flag to do a shallow sync, if you have downloaded${c0}\n" &&
	printf "${bold}${YEL}the Chromium repo with the --no-history flag.${c0}\n" &&
	printf "\n"
}

# --Shallow sync
gsyncShallow () {
	printf "\n" &&
	printf "${bold}${GRE}Running with the --shallow flag.${c0}\n" &&
	printf "\n" &&
	printf "${YEL}Rebasing/Syncing (with a depth of 1) and running hooks...\n" &&
	tput sgr0 &&
	
	cd $HOME/chromium/src/v8/ &&
	
	git checkout -f origin/main &&
	
	cd $HOME/chromium/src/third_party/devtools-frontend/src &&
	
	git checkout -f origin/main &&
	
	cd $HOME/chromium/src/third_party/ffmpeg &&
	
	git checkout -f origin/main &&

	cd $HOME/chromium/src &&
	
	rm -v -f $HOME/chromium/src/components/neterror/resources/images/default_100_percent/offline/favicon-16x16.png &&
	
	rm -v -f $HOME/chromium/src/components/neterror/resources/images/default_200_percent/offline/favicon-32x32.png &&
	
	rm -v -f $HOME/chromium/src/content/shell/app/thorium_shell.ico &&
	
	rm -v -f $HOME/chromium/src/chrome/browser/thorium_flag_entries.h &&
	
	rm -v -f $HOME/chromium/src/chrome/browser/thorium_flag_choices.h &&
	
	rm -v -r -f $HOME/chromium/src/third_party/pak &&
	
	git checkout -f origin/main &&
	
	git rebase-update --current &&
	
	gclient sync -R -D --no-history --shallow &&
	
	gclient runhooks &&
	
	printf "${YEL}Done!\n" &&
	printf "\n" &&
	
	printf "${YEL}Downloading PGO Profiles for Linux, Windows, and Mac...\n" &&
	printf "\n" &&
	tput sgr0 &&
	
	python3 tools/update_pgo_profiles.py --target=linux update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&
	
	python3 tools/update_pgo_profiles.py --target=win64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&
	
	python3 tools/update_pgo_profiles.py --target=mac update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&
	
	printf "\n" &&
	
	printf "${GRE}Done! ${YEL}You can now run ./setup.sh\n"
	tput sgr0 &&

	c0='\033[0m' # Reset Text
	c1='\033[0m\033[36m\033[1m' # Light Cyan
	c2='\033[0m\033[1;31m' # Light Red
	c3='\033[0m\033[37m' # Light Grey
	c4='\033[0m\033[1;34m\033[1m' # Light Blue
	c5='\033[0m\033[1;37m' # White
	c6='\033[0m\033[1;34m' # Dark Blue
	c7='\033[1;32m' # Green
	
	printf "\n" &&
	printf "${c4}                .,:loool:,.              \n" &&
	printf "${c4}            .,coooooooooooooc,.          \n" &&
	printf "${c4}         .,lllllllllllllllllllll,.       \n" &&
	printf "${c4}        ;ccccccccccccccccccccccccc;      \n" &&
	printf "${c1}      ,${c4}ccccccccccccccccccccccccccccc.    \n" &&
	printf "${c1}     ,oo${c4}c::::::::ok${c5}00000${c3}OOkkkkkkkkkkk:   \n" &&
	printf "${c1}    .ooool${c4};;;;:x${c5}K0${c6}kxxxxxk${c5}0X${c3}K0000000000.  \n" &&
	printf "${c1}    :oooool${c4};,;O${c5}K${c6}ddddddddddd${c5}KX${c3}000000000d  \n" &&
	printf "${c1}    lllllool${c4};l${c5}N${c6}dllllllllllld${c5}N${c3}K000000000  \n" &&
	printf "${c1}    lllllllll${c4}o${c5}M${c6}dccccccccccco${c5}W${c3}K000000000  \n" &&
	printf "${c1}    ;cllllllllX${c5}X${c6}c:::::::::c${c5}0X${c3}000000000d  \n" &&
	printf "${c1}    .ccccllllllO${c5}Nk${c6}c;,,,;cx${c5}KK${c3}0000000000.  \n" &&
	printf "${c1}     .cccccclllllxO${c5}OOOO0${c1}kx${c3}O0000000000;   \n" &&
	printf "${c1}      .:ccccccccllllllllo${c3}O0000000OOO,    \n" &&
	printf "${c1}        ,:ccccccccclllcd${c3}0000OOOOOOl.     \n" &&
	printf "${c1}          .::ccccccccc${c3}dOOOOOOOkx:.       \n" &&
	printf "${c1}            ..,::cccc${c3}xOOOkkko;.          \n" &&
	printf "${c1}               ..::${c3}dOkkxl:.              \n" &&
	printf "\n"
	printf "${c7}            Long Live Chromium\041\n${c0}\n"
}

case $1 in
	--help) displayHelp; exit 0;;
esac

case $1 in
	--shallow) gsyncShallow; exit 0;;
esac

printf "\n" &&
printf "${bold}${GRE}Script to Rebase/Sync Chromium repo on Linux.${c0}\n" &&
printf "\n" &&
printf "${YEL}Rebasing/Syncing and running hooks...\n" &&
tput sgr0 &&

cd $HOME/chromium/src/v8/ &&

git checkout -f origin/main &&

cd $HOME/chromium/src/third_party/devtools-frontend/src &&

git checkout -f origin/main &&

cd $HOME/chromium/src &&

rm -v -f $HOME/chromium/src/components/neterror/resources/images/default_100_percent/offline/favicon-16x16.png &&

rm -v -f $HOME/chromium/src/components/neterror/resources/images/default_200_percent/offline/favicon-32x32.png &&

rm -v -f $HOME/chromium/src/content/shell/app/thorium_shell.ico &&

rm -v -f $HOME/chromium/src/chrome/browser/thorium_flag_entries.h &&

rm -v -f $HOME/chromium/src/chrome/browser/thorium_flag_choices.h &&

rm -v -r -f $HOME/chromium/src/third_party/pak &&

git checkout -f origin/main &&

git rebase-update &&

git fetch --tags &&

gclient sync --with_branch_heads --with_tags -f -R -D &&

gclient runhooks &&

printf "${YEL}Done!\n" &&
printf "\n" &&

printf "${YEL}Downloading PGO Profiles for Linux, Windows, and Mac...\n" &&
printf "\n" &&
tput sgr0 &&

python3 tools/update_pgo_profiles.py --target=linux update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&

python3 tools/update_pgo_profiles.py --target=win64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&

python3 tools/update_pgo_profiles.py --target=mac update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&

printf "\n" &&

printf "${GRE}Done! ${YEL}You can now run ./setup.sh\n"
tput sgr0 &&

c0='\033[0m' # Reset Text
c1='\033[0m\033[36m\033[1m' # Light Cyan
c2='\033[0m\033[1;31m' # Light Red
c3='\033[0m\033[37m' # Light Grey
c4='\033[0m\033[1;34m\033[1m' # Light Blue
c5='\033[0m\033[1;37m' # White
c6='\033[0m\033[1;34m' # Dark Blue
c7='\033[1;32m' # Green

printf "\n" &&
printf "${c4}                .,:loool:,.              \n" &&
printf "${c4}            .,coooooooooooooc,.          \n" &&
printf "${c4}         .,lllllllllllllllllllll,.       \n" &&
printf "${c4}        ;ccccccccccccccccccccccccc;      \n" &&
printf "${c1}      ,${c4}ccccccccccccccccccccccccccccc.    \n" &&
printf "${c1}     ,oo${c4}c::::::::ok${c5}00000${c3}OOkkkkkkkkkkk:   \n" &&
printf "${c1}    .ooool${c4};;;;:x${c5}K0${c6}kxxxxxk${c5}0X${c3}K0000000000.  \n" &&
printf "${c1}    :oooool${c4};,;O${c5}K${c6}ddddddddddd${c5}KX${c3}000000000d  \n" &&
printf "${c1}    lllllool${c4};l${c5}N${c6}dllllllllllld${c5}N${c3}K000000000  \n" &&
printf "${c1}    lllllllll${c4}o${c5}M${c6}dccccccccccco${c5}W${c3}K000000000  \n" &&
printf "${c1}    ;cllllllllX${c5}X${c6}c:::::::::c${c5}0X${c3}000000000d  \n" &&
printf "${c1}    .ccccllllllO${c5}Nk${c6}c;,,,;cx${c5}KK${c3}0000000000.  \n" &&
printf "${c1}     .cccccclllllxO${c5}OOOO0${c1}kx${c3}O0000000000;   \n" &&
printf "${c1}      .:ccccccccllllllllo${c3}O0000000OOO,    \n" &&
printf "${c1}        ,:ccccccccclllcd${c3}0000OOOOOOl.     \n" &&
printf "${c1}          .::ccccccccc${c3}dOOOOOOOkx:.       \n" &&
printf "${c1}            ..,::cccc${c3}xOOOkkko;.          \n" &&
printf "${c1}               ..::${c3}dOkkxl:.              \n" &&
printf "\n"
printf "${c7}            Long Live Chromium\041\n${c0}\n" &&

exit 0
