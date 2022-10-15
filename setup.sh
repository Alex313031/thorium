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
	printf "${bold}${GRE}Script to copy Thorium source files over the Chromium source tree.${c0}\n" &&
	printf "${bold}${YEL}Use the --mac flag for MacOS builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --raspi flag for Raspberry Pi builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --woa flag for Windows on ARM builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --avx2 flag for AVX2 Builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --sse3 flag for SSE3 Builds.${c0}\n" &&
	printf "${bold}${YEL}IMPORTANT: For Polly builds, first run build_polly.sh in Thorium/infra, then use the setup_polly.sh${c0}\n" &&
	printf "${bold}${YEL}script in Thorium/other/Polly. Both of these actions should be taken AFTER running this script!${c0}\n" &&
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

printf "\n" &&
printf "${YEL}Copying Thorium source files over the Chromium tree...\n" &&
tput sgr0 &&

cp -r -v src/BUILD.gn $HOME/chromium/src/ &&
cp -r -v src/ash/. $HOME/chromium/src/ash/ &&
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
cp -r -v thorium_shell/. $HOME/chromium/src/out/thorium/ &&
cp -r -v pak_src/bin/pak $HOME/chromium/src/out/thorium/ &&
cp -r -v pak_src/bin/pak-win/. $HOME/chromium/src/out/thorium/ &&

echo " # Workaround for DevTools" &&
mkdir -v -p $HOME/chromium/src/out/thorium/gen/third_party/devtools-frontend/src/front_end/Images/ &&
cp -r -v src/third_party/devtools-frontend/src/front_end/Images/src/chromeSelectDark.svg $HOME/chromium/src/out/thorium/gen/third_party/devtools-frontend/src/front_end/Images/ &&

printf "\n" &&
printf "${YEL}Making some scripts executable...\n" &&
tput sgr0 &&
chmod -v +x $HOME/chromium/src/tools/clang/scripts/build.py &&

chmod -v +x $HOME/chromium/src/tools/gn/bootstrap/bootstrap.py &&

chmod -v +x $HOME/chromium/src/chrome/installer/linux/debian/build.sh &&

# MacOS Widevine Workaround
copyMacOS () {
	printf "\n" &&
	printf "${YEL}Copying files for MacOS...${c0}\n" &&
	cp -r -v other/Mac/cdm_registration.cc $HOME/chromium/src/chrome/common/media/ &&
	cp -r -v arm/mac_arm.gni $HOME/chromium/src/build/config/arm.gni &&
	printf "\n"
}

case $1 in
	--mac) copyMacOS;
esac

# Raspberry Pi Source Files
copyRaspi () {
	printf "\n" &&
	printf "${YEL}Copying Raspberry Pi build files...${c0}\n" &&
	cp -r -v arm/build/config/* $HOME/chromium/src/build/config/ &&
	cp -r -v arm/raspi_arm.gni $HOME/chromium/src/build/config/arm.gni &&
	printf "\n"
}

case $1 in
	--raspi) copyRaspi;
esac

# Windows on ARM workaround
copyWOA () {
	printf "\n" &&
	printf "${YEL}Copying Windows on ARM build files...${c0}\n" &&
	cp -r -v arm/build/config/* $HOME/chromium/src/build/config/ &&
	cp -r -v arm/woa_arm.gni $HOME/chromium/src/build/config/arm.gni &&
	printf "\n"
}

case $1 in
	--woa) copyWOA;
esac

# Copy AVX2 files
copyAVX2 () {
	printf "\n" &&
	printf "${YEL}Copying AVX2 build files...${c0}\n" &&
	cp -r -v other/AVX2/build/config/* $HOME/chromium/src/build/config/ &&
	cp -r -v other/AVX2/third_party/opus/src/* $HOME/chromium/src/third_party/opus/src/ &&
	printf "\n"
}

case $1 in
	--avx2) copyAVX2;
esac

# Copy SSE3 files
copySSE3 () {
	printf "\n" &&
	printf "${YEL}Copying SSE3 build files...${c0}\n" &&
	cp -r -v other/SSE3/build/config/* $HOME/chromium/src/build/config/ &&
	printf "\n"
}

case $1 in
	--sse3) copySSE3;
esac

printf "${GRE}Done!\n" &&
printf "\n" &&

printf "${YEL}Exporting variables and setting handy aliases.\n" &&

export NINJA_SUMMARIZE_BUILD=1 &&

export EDITOR=nano &&

export VISUAL=nano &&

alias origin='git checkout -f origin/main' &&

alias gfetch='git fetch --tags' &&

alias rebase='git rebase-update' &&

alias gsync='gclient sync --with_branch_heads --with_tags -f -R -D' &&

alias args='gn args out/thorium' &&

alias gnls='gn ls out/thorium' &&

alias show='git show-ref' &&

alias runhooks='gclient runhooks' &&

alias pgo='python3 tools/update_pgo_profiles.py --target=linux update --gs-url-base=chromium-optimization-profiles/pgo_profiles' &&

alias pgow='python3 tools/update_pgo_profiles.py --target=win64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles' &&

alias pgom='python3 tools/update_pgo_profiles.py --target=mac update --gs-url-base=chromium-optimization-profiles/pgo_profiles' &&
printf "\n" &&
tput sgr0 &&

printf "export ${CYA}NINJA_SUMMARIZE_BUILD=1${c0}\n" &&

printf "export ${CYA}EDITOR=nano${c0}\n" &&

printf "export ${CYA}VISUAL=nano${c0}\n" &&
printf "\n" &&

printf "alias ${YEL}origin${c0} = ${CYA}git checkout -f origin/main${c0}\n" &&

printf "alias ${YEL}gfetch${c0} = ${CYA}git fetch --tags${c0}\n" &&

printf "alias ${YEL}rebase${c0} = ${CYA}git rebase-update${c0}\n" &&

printf "alias ${YEL}gsync${c0} = ${CYA}gclient sync --with_branch_heads --with_tags -f -R -D${c0}\n" &&

printf "alias ${YEL}args${c0} = ${CYA}gn args out/thorium${c0}\n" &&

printf "alias ${YEL}gnls${c0} = ${CYA}gn ls out/thorium${c0}\n" &&

printf "alias ${YEL}show${c0} = ${CYA}git show-ref${c0}\n" &&

printf "alias ${YEL}runhooks${c0} = ${CYA}gclient runhooks${c0}\n" &&

printf "alias ${YEL}pgo${c0} = ${CYA}python3 tools/update_pgo_profiles.py --target=linux update --gs-url-base=chromium-optimization-profiles/pgo_profiles${c0}\n" &&

printf "alias ${YEL}pgow${c0} = ${CYA}python3 tools/update_pgo_profiles.py --target=win64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles${c0}\n" &&

printf "alias ${YEL}pgom${c0} = ${CYA}python3 tools/update_pgo_profiles.py --target=mac update --gs-url-base=chromium-optimization-profiles/pgo_profiles${c0}\n" &&

printf "${CYA}\n" &&

cat $HOME/thorium/logos/thorium_ascii_art.txt &&

# Display raspi ascii art
displayRaspi () {
	cat $HOME/thorium/logos/raspi_ascii_art.txt
}

case $1 in
	--raspi) displayRaspi;
esac

printf "${GRE}Enjoy Thorium!\n" &&
printf "\n" &&
tput sgr0

exit 0
