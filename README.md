# Thorium
Chromium fork for linux named after radioactive element No. 90

<img src="https://github.com/Alex313031/Thorium/blob/main/logos/product_logo_128.png">

# Chromium fork for Linux, named after radioactive element No. 90
-Always built with latest x64 'Trunk' build of chromium \
-Intended to behave the most like Google Chrome. \
-NOTE: The M69 is a legacy build for Debian Jessie and Ubuntu 12.04/14.04.

Includes widevine, ffmpeg, chrome plugins, as well as thinlto and pgo optimizations, and experimental MPEG-DASH and PDF annotation support.

args.gn exclude API Keys (you can get them yourself) and the pgo profile path is different for each chromium version. (See below.)

In general follow build instructions at https://chromium.googlesource.com/chromium/src/+/refs/heads/main/docs/linux/build_instructions.md and API Keys (if desired) at https://www.chromium.org/developers/how-tos/api-keys

# Building
- The file BUILD.gn goes in //chromium/src/build/config/compiler/ (replace when copying)
- The files within the "logos" dir in this repo go in **//chromium/src/chrome/app/theme/chromium/ & //chromium/src/chrome/app/theme/default_100_percent/chromium/ & //chromium/src/chrome/app/theme/default_200_percent/chromium/** (replace all when copying) 
- The file **BRANDING** goes in **//chromium/src/chrome/app/theme/chromium/** (replace when copying)
- The files **chromium_strings.grd** , **generated_resources.grd** , and **google_chrome_strings.grd** go in **//chromium/src/chrome/app/** (replace)
- After initial download of source code, run (all from within //chromium/src/) 'git rebase-update', then 'gclient sync --with_branch_heads --with_tags', then 'gclient runhooks', and then finally update the PGO profile file with the command 'python tools/update_pgo_profiles.py --target=linux update --gs-url-base=chromium-optimization-profiles/pgo_profiles'  The file will be downloaded to //chromium/src/chrome/build/pgo_profiles/*.profdata with the actual file name looking something like 'chrome-linux-main-1632505958-ddbb37bcdfa7dbd7b10cf3a9b6a5bc45e7a958a6.profdata', which should be added to the end of args.gn as per below.
- Run 'gn args out/Default' and the **contents of args.gn in this repo should be copy/pasted** --Include your api keys here at the top or leave blank, and edit the last line to the actual path and file name file name of '*.profdata'
- To build, run 'autoninja -j8 -C out/Default chrome -d stats' The -j# can be changed to limit or increase the number of jobs (generally should be the number of CPU cores on your machine), and the -d stats at the end just shows better verbose stats during compiling.
- To install, copy/paste the contents of your out/Default dir to a good location I.E. $HOME/bin/technetium. **RECOMMENDED - Copy and run clean.sh within this dir to clean up build artifacts**, then you can copy tc.sh and technetium.desktop to appropriate locations I.E. $HOME/bin/ and $HOME/.local/share/applications/, respectively. These three files are located in scripts_and_desktop within this repo.

Thanks to https://github.com/robrich999/ for some info that went into this project.

CUR_VERSION : Version 97.0.4773.1
