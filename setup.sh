#!/bin/bash

# Copyright (c) 2022 Alex313031.

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

# --help
displayHelp () {
	printf "\n" &&
	printf "${bold}${GRE}Script to copy Thorium source files over the Chromium source tree.${c0}\n" &&
	printf "\n"
}

case $1 in
	--help) displayHelp; exit 0;;
esac

printf "\n" &&
printf "${YEL}Creating build output directory...\n" &&
tput sgr0 &&

mkdir -v -p $HOME/chromium/src/out/thorium/ &&
printf "\n" &&

printf "${YEL}Copying Thorium source files over the Chromium tree...\n" &&
tput sgr0 &&

cp -r -v src/BUILD.gn $HOME/chromium/src/ &&
cp -r -v src/ash/. $HOME/chromium/src/ash/ &&
cp -r -v src/base/. $HOME/chromium/src/base/ &&
cp -r -v src/build/. $HOME/chromium/src/build/ &&
cp -r -v src/chrome/. $HOME/chromium/src/chrome/ &&
cp -r -v src/components/. $HOME/chromium/src/components/ &&
cp -r -v src/extensions/. $HOME/chromium/src/extensions/ &&
cp -r -v src/content/. $HOME/chromium/src/content/ &&
cp -r -v src/media/. $HOME/chromium/src/media/ &&
cp -r -v src/net/. $HOME/chromium/src/net/ &&
cp -r -v src/sandbox/. $HOME/chromium/src/sandbox/ &&
cp -r -v src/third_party/. $HOME/chromium/src/third_party/ &&
cp -r -v src/tools/. $HOME/chromium/src/tools/ &&
cp -r -v src/ui/. $HOME/chromium/src/ui/ &&
cp -r -v src/v8/. $HOME/chromium/src/v8/ &&
cp -r -v src/thorium_shell/. $HOME/chromium/src/out/thorium/ &&
cp -r -v src/pak_src/bin/pak $HOME/chromium/src/out/thorium/ &&
cp -r -v src/pak_src/bin/pak-win/. $HOME/chromium/src/out/thorium/ &&

# Workaround for DevTools
cp -r -v src/third_party/devtools-frontend/src/front_end/Images/src/chromeSelectDark.svg $HOME/chromium/src/out/thorium/gen/third_party/devtools-frontend/src/front_end/Images/ &&

printf "${GRE}Done!\n" &&
printf "\n" &&

printf "${YEL}Setting NINJA_SUMMARIZE_BUILD=1 and aliases to download the PGO Profiles for supported targets, and the gsync alias.\n" &&

export NINJA_SUMMARIZE_BUILD=1 &&

export EDITOR=nano &&

export VISUAL=nano &&

alias origin='git checkout -f origin/main' &&

alias rebase='git rebase-update' &&

alias args='gn args out/thorium' &&

alias gnls='gn ls out/thorium' &&

alias gsync='gclient sync --with_branch_heads --with_tags -f -R -D' &&

alias runhooks='gclient runhooks' &&

alias pgo='python3 tools/update_pgo_profiles.py --target=linux update --gs-url-base=chromium-optimization-profiles/pgo_profiles' &&

alias pgow='python3 tools/update_pgo_profiles.py --target=win64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles' &&

alias pgom='python3 tools/update_pgo_profiles.py --target=mac update --gs-url-base=chromium-optimization-profiles/pgo_profiles' &&

printf "\n" &&

printf "${YEL}Dropping you to ~/chromium/src\n" &&
printf "\n" &&

printf "${YEL}Look in this file to see the aliases and what they're for.\n" &&
printf "\n" &&

printf "${GRE}Enjoy Thorium!\n" &&
printf "\n" &&
tput sgr0 &&

cd ~/chromium/src &&

exit 0
