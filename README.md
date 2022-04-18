![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/alex313031/thorium?label=Version%3A) &nbsp;![GitHub](https://img.shields.io/github/license/alex313031/thorium?color=green&label=License%3A) &nbsp;![GitHub commit activity](https://img.shields.io/github/commit-activity/w/alex313031/thorium?color=blueviolet&label=Commit%20Activity%3A) &nbsp;![Subreddit subscribers](https://img.shields.io/reddit/subreddit-subscribers/ChromiumBrowser?style=social)
# Thorium

<img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/chrome_app_icon_192.png">

## Chromium fork for linux named after radioactive element No. 90. Windows/MacOS/Android/Other builds see below. NEW: New builds for different architectures in Thorium-Special! Also Google Sync is now enabled with baked in API Keys!
- Always built with the latest x64 tip-o-tree "Trunk" build of Chromium.
- Intended to behave like and have the featureset of Google Chrome, with differences/patches/enhancements listed below. \
&nbsp;&ndash; I also build for windows less frequently > https://github.com/Alex313031/Thorium-Win \
&nbsp;&ndash; AVX2 Builds for Windows and Linux > https://github.com/Alex313031/Thorium-AVX2 (source files for AVX2 are in that repo.) \
&nbsp;&ndash; Special and Experimental builds, sometimes for other CPUs (read the readme), and MacOS/Raspi are located at > https://github.com/Alex313031/Thorium-Special You can find builds for Piledriver/Steamroller/Excavator CPUS, Haswell CPUs, as well as SSE4, SSE4.2, and pure AVX builds. \
&nbsp;&ndash; __NEW__: Tigerlake (11th Gen) builds by @midzer \
&nbsp;&ndash; __NEW__: Thorium Special now also has experimental Android ARM32 & ARM64 builds. \
&nbsp;&ndash; __NEW__: Thorium Special now also has new ARM32 builds for things like the Raspberry Pi. \
&nbsp;&ndash; __NEW__: Thorium Special now also has ARM64 and x64 builds for MacOS.

&nbsp;&nbsp;&ndash; Includes Widevine, FFmpeg, Chrome Plugins, as well as thinLTO, CFlag, LLVM Loop, and PGO compiler optimizations. It is built with SSE4, and AVX, so it won't launch on CPU's below 2nd gen Core or AMD FX, but benefits from Advanced Vector EXtensions. You can disable this and use regular SSE3 like Chromium and Chrome, or builds from Thorium special of which some are SSE4 or SSE4.2. (See below.)

### FEATURES & DIFFERENCES BETWEEN CHROMIUM AND THORIUM <img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/bulb_32.png">
> - Various compiler flags that improve performance and target AVX CPU's (read patches.md)
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
> - Disable Google API Key warning (you can still use API Keys to enable sync) from Ungoogled Chromium.
> - Adds DuckDuckGo, Ecosia, Ask.com, and Yandex.com in US and other locales, along with the normal search engines.
> - Always use the local NTP (New Tab Page) regardless of search engine.
> - Fix icons when distilling page content in Reader Mode.
> - Enable new Menu UI when right clicking the Reload button. (Adds 'Normal Reload', 'Hard Reload', and 'Clear Cache and Hard Reload')
> - Home button shown by Default.
> - Prefetch settings updated to respect privacy.
> - Patches for GN and chrome_sandbox when building.
> - Remove the addition of the Chrome APT sources.list during installation.
> - Inclusion of pak (linux) and pack_mingw (windows), a utility for packing and unpacking the &#42;.pak files in Thorium and any other Chromium based browser.
> - Logo and Branding/Naming changed to the Thorium logo, Thorium name, and "Alex313031" being appended to "The Chromium Authors" in credits, etc.
> - .desktop file includes useful cmdline flags that enable experimental or useful features. (read PATCHES.md)
> - Includes installer patches and files to include ChromeDriver and content_shell, with a .desktop file being provided for content_shell (named thorium-shell.desktop and shows in desktop environments as Thorium Content Shell). You can also run content_shell with the command thorium-shell (custom wrapper for it, located in /usr/bin/). You can run ChromeDriver at /usr/bin/chromedriver. Also, patches for abseil library and mini_installer when building for AVX on Windows.
> - Right clicking the launcher after install gives two additional desktop actions, one to open content-shell, and another to open in safe mode which disables any flags one has set in chrome://flags until the next launch.
- For more info, read the PATCHES.md file.
- Known bugs are in the BUGS.md file.

&nbsp;&nbsp;&ndash; UPDATE: Thorium now has API Keys baked in but the instructions below are the same if you are building it yourself as I will not include the keys here. | args.gn exclude API Keys (you can get them yourself) and the PGO profile path is different for each Chromium version (only needed when building). (See below.)

# Building <img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/build_24.png">
_**The scripts assume the Chromium source is at $HOME/chromiums/src/. You may have to 'sudo chmod +x' the scripts to make them executable.**_ 
- For Windows AVX2 follow the instructions at https://github.com/Alex313031/Thorium-AVX2/ as well as these instructions.
- In general follow build instructions at https://chromium.googlesource.com/chromium/src/+/refs/heads/main/docs/linux/build_instructions.md and API Keys (if desired) at https://www.chromium.org/developers/how-tos/api-keys \
&nbsp;&nbsp;&ndash; For Windows builds, follow https://chromium.googlesource.com/chromium/src.git/+/HEAD/docs/win_cross.md This is for cross-compiling for Windows on Linux (what I do). For building natively on Windows, you will have to manually do the commands in `trunk.sh` and manually copy the files outlined in `setup.sh`, as well as using the windows_args.gn for your args.gn file (for both native and cross compiling), generally following instructions at https://chromium.googlesource.com/chromium/src/+/main/docs/windows_build_instructions.md
- After initial download of Chromium source code, run (from where you cloned this repo) `./trunk.sh`. This will update and sync the sources and at the end it will download the PGO profile for chromium. The file will be downloaded to *//chromium/src/chrome/build/pgo_profiles/&#42;.profdata* with the actual file name looking something like 'chrome-linux-main-1632505958-ddbb37bcdfa7dbd7b10cf3a9b6a5bc45e7a958a6.profdata', which should be added to the end of args.gn as per below.
- Then, (from where you cloned this repo) run `./setup.sh`. This will copy all the files and patches to the needed locations and drop you to *//chromium/src*.
- Run `export EDITOR=nano` & `export VISUAL=nano` *# You can substitute a cmdline editor like vim here, but many GUI editors cause it to try and parse the args.gn file before it is even saved.*
- Run `gn args out/thorium` and the contents of 'args.gn' in the 'misc' dir of this repo should be copy/pasted into the editor. Note that for Windows, Mac, ChromiumOS, or Android there are seperate &#42;_args.gn files for those platforms. *--Include your api keys here at the top or leave blank, and edit the last line to point to the actual path and file name of '&#42;.profdata'*
- 'misc/args.list' contains an alphabetical list with descriptions of all possible build arguments; gn_args.list gives a similar list but with the flags in args.gn added.
- To build, run `autoninja -C out/thorium chrome chromedriver chrome_sandbox content_shell -j8` For Windows, use `autoninja -C out/thorium chrome chromedriver content_shell setup mini_installer -j8` *The -j# can be changed to limit or increase the number of jobs (generally should be the number of CPU cores on your machine)*
- To install, copy/paste the contents of your *out/thorium* dir to a good location, i.e. *$HOME/bin/thorium*. **RECOMMENDED - Copy and run clean.sh within this dir to clean up build artifacts**. Then you can just run the browser with `~/bin/thorium/chrome`, the content_shell with `~/bin/thorium/content_shell`, or chromedriver with `~/bin/thorium/chromedriver`.
- **Proper Install:** To install with a .deb, dont copy the contents of *out/thorium*; instead run <br/> `autoninja -C out/thorium/ "chrome/installer/linux:unstable_deb"` A nice .deb file will now be in *out/thorium* and you can install it with `sudo dpkg -i *.deb` It will be called 'thorium-browser-unstable_$VERSIONNUMBER_amd64.deb', and will be installed to */opt/chromium.org/thorium-unstable/*. For Windows, just run the mini_installer.exe. \
&nbsp;&nbsp; NOTE: To get back to "Trunk", i.e. to revert all changes in order to build vanilla chromium, just run `./trunk.sh` again. \
&nbsp;&nbsp; NOTE: To compile without AVX, simply go to *//chromium/src/build/config/compiler/BUILD.gn*, search for *mavx*, and replace *mavx* with *msse3*.

https://www.reddit.com/r/ChromiumBrowser/ is a subreddit I made for Thorium and general Chromium discussion and https://alex313031.blogspot.com/ is a blog I made relating to Chromium/ChromiumOS. \
I also build ChromiumOS with codecs, linux-firmware/modules, and added packages at > https://github.com/Alex313031/ChromiumOS/

&minus; Thanks to https://github.com/robrich999/ for some info and fixes that went into this project.\
&minus; Thanks to https://github.com/midzer/ for support and helping with builds. \
&minus; Also thanks to https://github.com/bromite/bromite, https://github.com/saiarcot895/chromium-ubuntu-build, https://github.com/Eloston/ungoogled-chromium, and https://github.com/GrapheneOS/Vanadium for patch code. \
&minus; The pak_src dir, and the binaries in pack_src/bin are credited to @freeer https://github.com/myfreeer/chrome-pak-customizer/

&nbsp;&nbsp; NOTE: libpepflashplayer.so is included for posterity and can be used to enable Adobe Flash on older Chromium releases. ʘ‿ʘ

*Thanks for using Thorium!*

<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/Thorium90_502.jpg" width="200">

<img src="https://github.com/Alex313031/Thorium/blob/main/logos/OLD/GitHub-Mark-32px.png">
