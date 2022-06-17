![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/alex313031/thorium?label=Version%3A) &nbsp;![GitHub](https://img.shields.io/github/license/alex313031/thorium?color=green&label=License%3A) &nbsp;![GitHub commit activity](https://img.shields.io/github/commit-activity/w/alex313031/thorium?color=blueviolet&label=Commit%20Activity%3A) &nbsp;![Subreddit subscribers](https://img.shields.io/reddit/subreddit-subscribers/ChromiumBrowser?style=social)
# Thorium

<img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/thorium_ver_2048_grey_old.png">

## Chromium fork for linux named [after radioactive element No. 90](https://en.wikipedia.org/wiki/Thorium). Windows/MacOS/RasPi/Other builds see below.

- Always built with the latest x64 tip-o-tree "Trunk" build of [Chromium](https://www.chromium.org/).
- Intended to behave like and have the featureset of Google Chrome, with differences/patches/enhancements listed below.
- Includes Widevine, All Codecs, Chrome Plugins, as well as thinLTO, CFlag, LDFlag, LLVM Loop, and PGO compiler optimizations. It is built with SSE4, AVX, and AES, so it won't launch on CPU's below 2nd gen Core or AMD FX, but benefits from Advanced Vector EXtensions. You can disable this and use regular SSE3 like Chromium and Chrome, or builds from Thorium special, some of which are SSE4. (See below.)
### Other Builds &nbsp;<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/winflag_animated.gif" width="34"> &nbsp;<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/AVX2.png" width="48"> &nbsp;<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/apple.png" width="30"> &nbsp;<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/Raspberry_Pi_Logo.svg" width="24">
&nbsp;&ndash; I also build for Windows > [Thorium Win](https://github.com/Alex313031/Thorium-Win) You can also find SSE4 builds for Windows here.\
&nbsp;&ndash; AVX2 Builds for Windows and Linux > [Thorium AVX2](https://github.com/Alex313031/Thorium-AVX2) (source files for AVX2 are in that repo.) \
&nbsp;&ndash; MacOS (M1 and X64) and Raspberry Pi (arm64) Builds, as well as Special and Experimental builds, sometimes for other CPUs (read the Readme), are located at > [Thorium Special](https://github.com/Alex313031/Thorium-Special) You can also find builds for Piledriver/Steamroller/Excavator CPUS, Haswell CPUs, as well as SSE4 Linux builds. \
&nbsp;&ndash; __NEW__: Thorium Website with deb repo for auto-updating on Linux > https://thorium.rocks/ \
&nbsp;&ndash; __NEW__: Tigerlake (11th Gen) builds by @midzer in Thorium Special

### FEATURES & DIFFERENCES BETWEEN CHROMIUM AND THORIUM <img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/bulb_light.svg#gh-dark-mode-only"> <img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/bulb_dark.svg#gh-light-mode-only">
> - Various compiler flags that improve performance and target AVX CPU's (read [PATCHES.md](https://github.com/Alex313031/Thorium/blob/main/PATCHES.md))
> - Experimental MPEG-DASH.
> - JPEG XL Image File Format support.
> - Enable Live Caption (SODA) on all builds.
> - Experimental PDF annotation support (called "Ink" on ChromiumOS). # DISABLED FOR NOW BECAUSE OF CRASHES.
> - Patches from Debian including font rendering patch, VAAPI Patch, Intel HD support patch, native notifications patch, title bar patch, and... the VDPAU Patch!! (Rejoice Nvidia users)
> - Audio Sandbox patch.
> - DoH (DNS over HTTPS) patches from Bromite.
> - Enable Do Not Track by default patch from Vanadium.
> - Show full URLs including trailing slashes in address bar by default.
> - Disable FLOC patch.
> - Disable annoying Google API Key Infobar warning (you can still use API Keys to enable sync) from Ungoogled Chromium.
> - Disable annoying Default Browser Infobar warning.
> - Adds DuckDuckGo, Ecosia, Ask.com, and Yandex.com in US and other locales, along with the normal search engines.
> - Always use the local NTP (New Tab Page) regardless of search engine.
> - Fix icons when distilling page content in Reader Mode.
> - Enable new Menu UI when right clicking the Reload button. (Adds 'Normal Reload', 'Hard Reload', and 'Clear Cache and Hard Reload')
> - Home button and Chrome Labs shown by Default.
> - Prefetch settings updated to respect privacy.
> - Patches for GN and chrome_sandbox when building.
> - Remove the addition of the Chrome APT sources.list during installation.
> - Widevine CDM Patch for Linux.
> - Enable Parallel Downloading by Default.
> - Inclusion of <img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/pak.png" width="16"> pak (linux) and pak_mingw (windows), a utility for packing and unpacking the &#42;.pak files in Thorium and any other Chromium based browser.
> - Logo and Branding/Naming changed to the Thorium logo, Thorium name, and "Alex313031" being appended to "The Chromium Authors" in credits, etc.
> - .desktop file includes useful cmdline flags that enable experimental or useful features. (read PATCHES.md)
> - Includes installer patches and files to include ChromeDriver and content_shell (thorium_shell), with a .desktop file being provided for content_shell (named thorium-shell.desktop and shows in desktop environments as Thorium Content Shell). These are also included in the Windows releases, but it doesn't make a shorcut, although a .png and .ico is in the install directory for you to make your own shortcut with an icon. You can also run content_shell with the command thorium-shell (custom wrapper for it, located in /usr/bin/). You can run ChromeDriver at /usr/bin/chromedriver or chromedriver.exe onWwindows. Also, patches for abseil library and mini_installer when building with AVX on Windows.
> - Right clicking the launcher after install gives three additional desktop actions, one to open thorium-shell, another to open in Safe Mode which disables any flags one has set in chrome://flags until the next launch, and lastly to open in Dark Mode which appends the --force-dark-mode flag.
- For more info, read the [PATCHES.md](https://github.com/Alex313031/Thorium/blob/main/PATCHES.md) file.
- Known bugs are in the [BUGS.md](https://github.com/Alex313031/Thorium/blob/main/BUGS.md) file.
- A list of Chromium command line flags can be found at > https://peter.sh/experiments/chromium-command-line-switches

&nbsp;&nbsp;&ndash; UPDATE: Thorium now has API Keys baked in but the instructions below are the same if you are building it yourself as I will not include the keys here. | args.gn exclude API Keys (you can get them yourself) and the PGO profile path is different for each Chromium version (only needed when building). (See below.)

## Building <img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/build_light.svg#gh-dark-mode-only"> <img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/build_dark.svg#gh-light-mode-only">
_**The scripts assume the Chromium source is at $HOME/chromiums/src/ and Thorium is at $HOME/thorium/. You may have to 'sudo chmod +x' the scripts to make them executable.**_ 
- UPDATE: For Windows and Windows [AVX2](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#Advanced_Vector_Extensions_2), I made new dedicated instructions. If you are building on Windows use > [WIN_INSTRUCTIONS.txt](https://github.com/Alex313031/Thorium/blob/main/misc/WIN_INSTRUCTIONS.txt) and if you are building for Windows on Linux, use > [WIN_CROSS_BUILD_INSTRUCTIONS.txt](https://github.com/Alex313031/Thorium/blob/main/misc/WIN_CROSS_BUILD_INSTRUCTIONS.txt)
- In general we follow build instructions at https://chromium.googlesource.com/chromium/src/+/refs/heads/main/docs/linux/build_instructions.md and API Keys (if desired) at https://www.chromium.org/developers/how-tos/api-keys 
- Make sure dependencies are installed by running ./chromium/src/build/install-build-deps.sh.
- After initial download of Chromium source code, run (from where you cloned this repo) `./trunk.sh`. This will update and sync the sources and at the end it will download the PGO profile for Chromium for all platforms. The file will be downloaded to *//chromium/src/chrome/build/pgo_profiles/&#42;.profdata* with the actual file name looking something like 'chrome-linux-main-1632505958-ddbb37bcdfa7dbd7b10cf3a9b6a5bc45e7a958a6.profdata', which should be added to the end of args.gn as per below.
- Then, (from where you cloned this repo) run `./setup.sh`. This will copy all the files and patches to the needed locations and drop you to *//chromium/src*.
- Run `gn args out/thorium` and the contents of '[args.gn](https://github.com/Alex313031/Thorium/blob/main/args.gn)' in the root of this repo should be copy/pasted into the editor. Note that for Windows, Mac, ChromiumOS, or Android there are seperate &#42;_args.gn files for those platforms. *--Include your api keys here at the top or leave blank, and edit the last line to point to the actual path and file name of '&#42;.profdata'* 
- For more info about args.gn, read the [ABOUT_GN_ARGS.md](https://github.com/Alex313031/Thorium/blob/main/infra/DEBUG/ABOUT_GN_ARGS.md) file.
- '[infra/args.list](https://github.com/Alex313031/Thorium/blob/main/infra/args.list)' contains an alphabetical list with descriptions of all possible build arguments; [gn_args.list](https://github.com/Alex313031/Thorium/blob/main/infra/gn_args.list) gives a similar list but with the flags in args.gn added.
- To build, run `./build.sh` (--help for help). For Windows, use `build_win.sh` or `autoninja -C out\thorium chrome chromedriver thorium_shell setup mini_installer -j8` *The -j# can be changed to limit or increase the number of jobs (generally should be the number of CPU cores on your machine)*
- To install, copy/paste the contents of your *out/thorium* dir to a good location, i.e. *$HOME/bin/thorium*. **RECOMMENDED - Copy and run clean.sh within this dir to clean up build artifacts**. Then you can just run the browser with `~/bin/thorium/chrome`, the content_shell with `~/bin/thorium/thorium_shell`, or chromedriver with `~/bin/thorium/chromedriver`.
- **Proper Install:** To install with a .deb, dont copy the contents of *out/thorium*; instead run <br/> `./thordeb.sh` (--help for help). A nice .deb file will now be in *out/thorium* and you can install it with `sudo dpkg -i *.deb` It will be called 'thorium-browser-unstable_$VERSIONNUMBER_amd64.deb', and will be installed to */opt/chromium.org/thorium-unstable/*. For Windows, just run the mini_installer.exe. \
- **Appimage:** You can also make an [Appimage](https://appimage.org/) of Thorium after making the .deb. \
See > https://github.com/Alex313031/Thorium/tree/main/infra/APPIMAGE#readme \

&nbsp;&nbsp; __NOTE:__ To get back to "Trunk", i.e. to revert all changes in order to build vanilla Chromium or to update your checkout, just run `./trunk.sh` again. \
&nbsp;&nbsp; __NOTE:__ To compile without AVX, simply go to *//chromium/src/build/config/compiler/BUILD.gn*, search for *avx*, and replace *avx* with *sse3* or *sse4*.

## Debugging <img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/bug.svg" width="28">
See > https://github.com/Alex313031/Thorium/tree/main/infra/DEBUG#readme

-------
https://www.reddit.com/r/ChromiumBrowser/ is a subreddit I made for Thorium and general Chromium discussion and https://alex313031.blogspot.com/ is a blog I made relating to Thorium/ThoriumOS. \
I also build ChromiumOS (now called ThoriumOS) with Thorium, Codecs, Widevine, linux-firmware/modules, and extra packages at > https://github.com/Alex313031/ChromiumOS/

&minus; Thanks to https://github.com/robrich999/ for some info and fixes that went into this project.\
&minus; Thanks to https://github.com/midzer/ for support and helping with builds. \
&minus; Also thanks to https://github.com/bromite/bromite, https://github.com/saiarcot895/chromium-ubuntu-build, https://github.com/Eloston/ungoogled-chromium, https://github.com/GrapheneOS/Vanadium, and https://github.com/iridium-browser/iridium-browser for patch code. \
&minus; The pak_src dir, and the binaries in pack_src/bin are credited to @freeer https://github.com/myfreeer/chrome-pak-customizer/

&nbsp;&nbsp; __NOTE:__ libpepflashplayer.so is included for posterity and can be used to enable Adobe Flash on older Chromium releases. ʘ‿ʘ

*Thanks for using Thorium!*

<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/Thorium90_504.jpg" width="200">

<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/GitHub/GitHub-Mark-Light-32px.png#gh-dark-mode-only"> <img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/GitHub/GitHub-Mark-32px.png#gh-light-mode-only">
