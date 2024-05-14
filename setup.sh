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
	printf "${bold}${GRE}Script to copy Thorium source files over the Chromium source tree.${c0}\n" &&
	printf "${bold}${YEL}Use the --mac flag for MacOS builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --raspi flag for Raspberry Pi builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --woa flag for Windows on ARM builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --avx2 flag for AVX2 Builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --sse4 flag for SSE4.1 Builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --sse3 flag for SSE3 Builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --sse2 flag for 32 bit SSE2 Builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --android flag for Android Builds.${c0}\n" &&
	printf "${bold}${YEL}Use the --cros flag for ChromiumOS Builds.${c0}\n" &&
	printf "${bold}${YEL}IMPORTANT: For Polly builds, first run build_polly.sh in ./infra before building.${c0}\n" &&
	printf "${bold}${YEL} This should be done AFTER running this setup.sh script!${c0}\n" &&
	printf "\n"
	printf "${bold}${YEL}NOTE: The \`CR_DIR\` env variable can be used to override the location of \"chromium/src\".${c0}\n" &&
	printf "${bold}${YEL} The default is $HOME/chromium/src${c0}\n" &&
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
printf "${YEL}Creating build output directory...\n" &&
tput sgr0 &&

mkdir -v -p ${CR_SRC_DIR}/out/thorium/ &&
printf "\n" &&

printf "\n" &&
printf "${YEL}Copying Thorium source files over the Chromium tree...${c0}\n" &&

# Copy libjxl src
cd ~/thorium &&
cp -r -v thorium-libjxl/src/. ${CR_SRC_DIR}/ &&

# Copy Thorium sources
cp -r -v src/BUILD.gn ${CR_SRC_DIR}/ &&
cp -r -v src/ash ${CR_SRC_DIR}/ &&
cp -r -v src/build ${CR_SRC_DIR}/ &&
cp -r -v src/chrome ${CR_SRC_DIR}/ &&
cp -r -v src/chromeos ${CR_SRC_DIR}/ &&
cp -r -v src/components ${CR_SRC_DIR}/ &&
cp -r -v src/content ${CR_SRC_DIR}/ &&
cp -r -v src/extensions ${CR_SRC_DIR}/ &&
cp -r -v src/media ${CR_SRC_DIR}/ &&
cp -r -v src/net ${CR_SRC_DIR}/ &&
cp -r -v src/sandbox ${CR_SRC_DIR}/ &&
cp -r -v src/services ${CR_SRC_DIR}/ &&
cp -r -v src/third_party ${CR_SRC_DIR}/ &&
cp -r -v src/tools ${CR_SRC_DIR}/ &&
cp -r -v src/ui ${CR_SRC_DIR}/ &&
cp -r -v src/v8 ${CR_SRC_DIR}/ &&

cp -r -v thorium_shell/. ${CR_SRC_DIR}/out/thorium/ &&
cp -r -v pak_src/binaries/pak ${CR_SRC_DIR}/out/thorium/ &&
cp -r -v pak_src/binaries/pak-win/. ${CR_SRC_DIR}/out/thorium/ &&

patchFFMPEG () {
	cp -v other/add-hevc-ffmpeg-decoder-parser.patch ${CR_SRC_DIR}/third_party/ffmpeg/ &&
	cp -v other/fix-policy-templates.patch ${CR_SRC_DIR}/ &&
	cp -v other/ftp-support-thorium.patch ${CR_SRC_DIR}/ &&

	printf "\n" &&
	printf "${YEL}Patching FFMPEG for HEVC...${c0}\n" &&
	cd ${CR_SRC_DIR}/third_party/ffmpeg &&
	git apply --reject ./add-hevc-ffmpeg-decoder-parser.patch &&

	printf "\n" &&
	printf "${YEL}Patching policy templates...${c0}\n" &&
	cd ${CR_SRC_DIR} &&
	git apply --reject ./fix-policy-templates.patch &&

	printf "\n" &&
	printf "${YEL}Patching FTP support...${c0}\n" &&
	cd ${CR_SRC_DIR} &&
	git apply --reject ./ftp-support-thorium.patch
}
[ -f ${CR_SRC_DIR}/third_party/ffmpeg/add-hevc-ffmpeg-decoder-parser.patch ] || patchFFMPEG;

cd ~/thorium &&

printf "\n" &&
echo "Copying other files to \`out/thorium\`" &&

# Add default_apps dir for Google Docs Offline extension.
mkdir -v -p ${CR_SRC_DIR}/out/thorium/default_apps &&
cp -r -v infra/default_apps/. ${CR_SRC_DIR}/out/thorium/default_apps/ &&

# Add initial preferences file to open Thorium welcome page on first run.
cp -v infra/initial_preferences ${CR_SRC_DIR}/out/thorium/ &&
cp -v infra/thor_ver ${CR_SRC_DIR}/out/thorium/ &&

# MacOS ARMv8.3-A optimizations
copyMacOS () {
	printf "\n" &&
	printf "${YEL}Copying files for MacOS...${c0}\n" &&
	cp -v arm/mac_arm.gni ${CR_SRC_DIR}/build/config/arm.gni &&
	cp -v other/AVX2/build/config/compiler/BUILD.gn ${CR_SRC_DIR}/build/config/compiler/ &&
	cp -r -v arm/third_party/* ${CR_SRC_DIR}/third_party/ &&
	printf "\n"
}
case $1 in
	--mac) copyMacOS;
esac

# Raspberry Pi Source Files
copyRaspi () {
	printf "\n" &&
	printf "${YEL}Copying Raspberry Pi build files...${c0}\n" &&
	cp -r -v arm/build/* ${CR_SRC_DIR}/build/ &&
	cp -r -v arm/media/* ${CR_SRC_DIR}/media/ &&
	cp -r -v arm/third_party/* ${CR_SRC_DIR}/third_party/ &&
	cp -r -v arm/raspi/* ${CR_SRC_DIR}/ &&
	cp -v pak_src/binaries/pak_arm64 ${CR_SRC_DIR}/out/thorium/pak &&
	#./infra/fix_libaom.sh &&
	printf "\n" &&
	cp -r -v arm/raspi/build/config/* ${CR_SRC_DIR}/build/config/ &&
	printf "\n" &&
	# Display raspi ascii art
	cat logos/raspi_ascii_art.txt
}
case $1 in
	--raspi) copyRaspi;
esac

# Windows on ARM64 files
copyWOA () {
	printf "\n" &&
	printf "${YEL}Copying Windows on ARM build files...${c0}\n" &&
	cp -r -v arm/build/* ${CR_SRC_DIR}/build/ &&
	cp -r -v arm/third_party/* ${CR_SRC_DIR}/third_party/ &&
	# Use regular arm.gni from src, pending further testing
	# cp -v arm/woa_arm.gni ${CR_SRC_DIR}/build/config/arm.gni &&
	printf "\n"
}
case $1 in
	--woa) copyWOA;
esac

# Copy AVX2 files
copyAVX2 () {
	printf "\n" &&
	printf "${YEL}Copying AVX2 build files...${c0}\n" &&
	cp -r -v other/AVX2/build/config/* ${CR_SRC_DIR}/build/config/ &&
	cp -r -v other/AVX2/third_party/* ${CR_SRC_DIR}/third_party/ &&
	cp -v other/AVX2/thor_ver ${CR_SRC_DIR}/out/thorium/ &&
	printf "\n"
}
case $1 in
	--avx2) copyAVX2;
esac

# Copy SSE4.1 files
copySSE4 () {
	printf "\n" &&
	printf "${YEL}Copying SSE4.1 build files...${c0}\n" &&
	cp -r -v other/SSE4.1/build/config/* ${CR_SRC_DIR}/build/config/ &&
	cp -v other/SSE4.1/thor_ver ${CR_SRC_DIR}/out/thorium/ &&
	printf "\n"
}
case $1 in
	--sse4) copySSE4;
esac

# Copy SSE3 files
copySSE3 () {
	printf "\n" &&
	printf "${YEL}Copying SSE3 build files...${c0}\n" &&
	cp -r -v other/SSE3/build/config/* ${CR_SRC_DIR}/build/config/ &&
	cp -v other/SSE3/thor_ver ${CR_SRC_DIR}/out/thorium/ &&
	printf "\n"
}
case $1 in
	--sse3) copySSE3;
esac

# Copy SSE2 files
copySSE2 () {
	printf "\n" &&
	printf "${YEL}Copying SSE2 (32-bit) build files...${c0}\n" &&
	cp -r -v other/SSE2/build/config/* ${CR_SRC_DIR}/build/config/ &&
	cp -v other/SSE2/thor_ver ${CR_SRC_DIR}/out/thorium/ &&
	printf "\n"
}
case $1 in
	--sse2) copySSE2;
esac

# Copy Android files
copyAndroid () {
	printf "\n" &&
	printf "${YEL}Copying Android (ARM64 and ARM32) build files...${c0}\n" &&
	cp -r -v arm/build/* ${CR_SRC_DIR}/build/ &&
	cp -r -v arm/media/* ${CR_SRC_DIR}/media/ &&
	cp -r -v arm/third_party/* ${CR_SRC_DIR}/third_party/ &&
	printf "\n" &&
	cp -r -v arm/android/* ${CR_SRC_DIR}/ &&
	printf "\n" &&
	#cp -r -v arm/android/third_party/* ${CR_SRC_DIR}/third_party/ &&
	rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_base/drawable-v26/ic_launcher.xml &&
	rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_base/drawable-v26/ic_launcher_round.xml &&
	rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-mdpi/layered_app_icon_background.png &&
	#rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-mdpi/layered_app_icon.png &&
	rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-xhdpi/layered_app_icon_background.png &&
	#rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-xhdpi/layered_app_icon.png &&
	rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-xxxhdpi/layered_app_icon_background.png &&
	#rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-xxxhdpi/layered_app_icon.png &&
	rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-nodpi/layered_app_icon_foreground.xml &&
	rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-hdpi/layered_app_icon_background.png &&
	#rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-hdpi/layered_app_icon.png &&
	rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-xxhdpi/layered_app_icon_background.png &&
	#rm -v -f ${CR_SRC_DIR}/chrome/android/java/res_chromium_base/mipmap-xxhdpi/layered_app_icon.png &&
	#./infra/fix_libaom.sh &&
	printf "\n" &&
	printf "${YEL}Downloading PGO profiles...${c0}\n" &&
	cd ${CR_SRC_DIR} &&
	python3 tools/update_pgo_profiles.py --target=android-arm64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&
	python3 tools/update_pgo_profiles.py --target=android-arm32 update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&
	cd ~/thorium &&
	printf "\n"
}
case $1 in
	--android) copyAndroid;
esac

# Copy CrOS files
copyCros () {
	printf "\n" &&
	printf "${YEL}Copying ChromiumOS build files...${c0}\n" &&
	cp -r -v other/CrOS/* ${CR_SRC_DIR}/ &&
	printf "\n"
}
case $1 in
	--cros) copyCros;
esac

printf "${GRE}Done!\n" &&
printf "\n" &&

printf "${YEL}Exporting variables and setting handy aliases...${c0}\n" &&

. ~/thorium/aliases &&

printf "\n" &&
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
printf "alias ${YEL}pgomac-arm${c0} = ${CYA}python3 tools/update_pgo_profiles.py --target=mac-arm update --gs-url-base=chromium-optimization-profiles/pgo_profiles${c0}\n" &&
printf "\n" &&

cat logos/thorium_ascii_art.txt &&

printf "${GRE}Enjoy Thorium!\n" &&
printf "\n" &&
tput sgr0
