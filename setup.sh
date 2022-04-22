#!/bin/bash

# Copyright (c) 2022 Alex313031.

YEL='\033[1;33m' # Yellow

printf "\n" &&
printf "${YEL}Creating build output directory...\n" &&
tput sgr0 &&

mkdir -v -p $HOME/chromium/src/out/thorium/ &&
printf "\n" &&

printf "${YEL}Copying source files over Chromium tree...\n" &&
tput sgr0 &&

cp -r -v build/. $HOME/chromium/src/build/ &&
cp -r -v chrome/. $HOME/chromium/src/chrome/ &&
cp -r -v components/. $HOME/chromium/src/components &&
cp -r -v content/. $HOME/chromium/src/content/ &&
cp -r -v media/. $HOME/chromium/src/media/ &&
cp -r -v net/. $HOME/chromium/src/net/ &&
cp -r -v sandbox/. $HOME/chromium/src/sandbox/ &&
cp -r -v third_party/. $HOME/chromium/src/third_party/ &&
cp -r -v tools/. $HOME/chromium/src/tools/ &&
cp -r -v ui/. $HOME/chromium/src/ui/ &&
cp -r -v v8/. $HOME/chromium/src/v8/ &&
cp -r -v content_shell/. $HOME/chromium/src/out/thorium/ &&
cp -r -v pak_src/bin/pak $HOME/chromium/src/out/thorium/ &&
cp -r -v pak_src/bin/pak-win/. $HOME/chromium/src/out/thorium/ &&

printf "${YEL}Done!\n" &&
printf "\n" &&

printf "${YEL}Setting NINJA_SUMMARIZE_BUILD=1 and aliases to download the PGO Profiles for supported targets, and the gsync alias.\n" &&

export NINJA_SUMMARIZE_BUILD=1 &&

export EDITOR=nano &&

export VISUAL=nano &&

alias gsync='gclient sync --with_branch_heads --with_tags -f -R -D' &&

alias pgo='python3 tools/update_pgo_profiles.py --target=linux update --gs-url-base=chromium-optimization-profiles/pgo_profiles' &&

alias pgow='python3 tools/update_pgo_profiles.py --target=win64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles' &&

alias pgom='python3 tools/update_pgo_profiles.py --target=mac update --gs-url-base=chromium-optimization-profiles/pgo_profiles' &&

printf "\n" &&

printf "${YEL}Dropping you to ~/chromium/src.\n" &&
printf "\n" &&

printf "${YEL}Look in this file to see the aliases and what they're for.\n" &&
printf "\n" &&

printf "${YEL}Enjoy Thorium!\n" &&
printf "\n" &&
tput sgr0 &&

cd $HOME/chromium/src &&

exit 0
