[![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/alex313031/thorium?label=Version%3A)](https://github.com/Alex313031/thorium/releases)
[![GitHub](https://img.shields.io/github/license/alex313031/thorium?color=green&label=License%3A)](https://github.com/Alex313031/thorium/blob/main/LICENSE.md)
[![GitHub commit activity](https://img.shields.io/github/commit-activity/w/alex313031/thorium?color=blueviolet&label=Commit%20Activity%3A)](https://github.com/Alex313031/thorium/commits/main/)
[![Subreddit subscribers](https://img.shields.io/reddit/subreddit-subscribers/ChromiumBrowser?style=social)](https://www.reddit.com/r/ChromiumBrowser/)

# Thorium

<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/thorium_ver_2048_grey_old.png">

## Chromium fork for linux named after [radioactive element No. 90](https://en.wikipedia.org/wiki/Thorium). Windows/MacOS/RasPi/Android/Other builds see below.

- Always built with the latest [LTS](https://support.google.com/chrome/a/answer/12239814) version of [Chromium](https://www.chromium.org/).
- Intended to behave like and have the featureset of Google Chrome, with differences/patches/enhancements listed below.
- Includes [Widevine](https://www.widevine.com/), [All Codecs](https://tools.woolyss.com/html5-audio-video-tester/), Chrome Plugins, as well as thinLTO, CFlag, LDFlag, LLVM Loop, and PGO compiler optimizations. It is built with [SSE4](https://en.wikipedia.org/wiki/SSE4), [AVX](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions), and [AES](https://en.wikipedia.org/wiki/AES_instruction_set), so it won't launch on CPU's below 2nd gen Core or AMD FX, but benefits from Advanced Vector EXtensions. 
  If your CPU lacks AVX, you can use builds from the [Thorium SSE3](https://github.com/Alex313031/Thorium-SSE3) repo.
### Other Builds &nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/winflag_animated.gif" width="34"> &nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/AVX2.png" width="48"> &nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/apple.png" width="30"> &nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/Android_Robot.svg" width="26"> &nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/Raspberry_Pi_Logo.svg" width="24"> &nbsp;<img src="https://raw.githubusercontent.com/Alex313031/thorium-win7/main/logos/STAGING/win7/compatible-with-windows-7.png" width="28">
&nbsp;&ndash; Windows builds are here > [Thorium Win](https://github.com/Alex313031/Thorium-Win) \
&nbsp;&ndash; AVX2 Builds for Windows and Linux > [Thorium AVX2](https://github.com/Alex313031/Thorium-AVX2) \
&nbsp;&ndash; MacOS (M1 and X64) builds are located at > [Thorium Mac](https://github.com/Alex313031/Thorium-Mac) \
&nbsp;&ndash; Android (arm32 & arm64) builds are located at > [Thorium Android](https://github.com/Alex313031/Thorium-Android) I might also occasionally post x86 builds. \
&nbsp;&ndash; Raspberry Pi builds are located at > [Thorium Raspi](https://github.com/Alex313031/Thorium-Raspi) For the Pi 3B/3B+ and Pi 4/400. \
&nbsp;&ndash; Special builds are located at > [Thorium Special](https://github.com/Alex313031/Thorium-Special) You can find SSE3/SSE4 and 32 bit builds for CPUs without AVX here. \
&nbsp;&ndash; Thorium Website with deb repo for auto-updating on Linux > https://thorium.rocks/ \
&nbsp;&ndash; __NEW__: Windows 7 / 8 / 8.1 / Server 2012 builds in > [Thorium Win7](https://github.com/Alex313031/thorium-win7)

### FEATURES & DIFFERENCES BETWEEN CHROMIUM AND THORIUM <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/bulb_light.svg#gh-dark-mode-only"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/bulb_dark.svg#gh-light-mode-only">
> - Various compiler flags that improve performance and target [AVX](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions) CPU's (read [PATCHES.md](https://github.com/Alex313031/thorium/blob/main/docs/PATCHES.md))
> - Experimental [MPEG-DASH](https://en.wikipedia.org/wiki/Dynamic_Adaptive_Streaming_over_HTTP) support.
> - HEVC/H.265 support on Linux and Windows.
> - [JPEG XL](https://jpeg.org/jpegxl/) Image File Format support restored and turned on by default.
> - [FTP](https://en.wikipedia.org/wiki/File_Transfer_Protocol) URL support restored and enabled by default.
> - Enable [Live Caption](https://support.google.com/chrome/answer/10538231?hl) (SODA) on all builds.
> - Experimental PDF annotation support (called "Ink" on ChromiumOS). # DISABLED FOR NOW BECAUSE OF CRASHES.
> - Patches from Debian including font rendering patch, VAAPI Patch, Intel HD support patch, native notifications patch, title bar patch, and... the VDPAU Patch!! (Rejoice Nvidia users)
> - Linux audio sandbox patch.
> - [DoH](https://en.wikipedia.org/wiki/DNS_over_HTTPS) (DNS over HTTPS) patches from Bromite.
> - Enable [Do Not Track](https://allaboutdnt.com/) by default patch from Vanadium.
> - Show full URLs including trailing slashes in address bar by default.
> - Disable [FLOC](https://en.wikipedia.org/wiki/Federated_Learning_of_Cohorts) patch.
> - Disable annoying Google API Key Infobar warning (you can still use API Keys to enable sync) from Ungoogled Chromium.
> - Disable annoying Default Browser Infobar warning.
> - Disable annoying IPH (In Product Help) promos.
> - Adds [DuckDuckGo](https://duckduckgo.com/), [Brave Search](https://search.brave.com/), [Ecosia](https://www.ecosia.org/), [Ask.com](https://www.ask.com/), and [Yandex.com](https://yandex.com/) in US and other locales, along with the normal search engines.
> - Always use the local NTP (New Tab Page) regardless of search engine.
> - Fix icons when distilling page content in [Reader Mode](https://www.howtogeek.com/423643/how-to-use-google-chromes-hidden-reader-mode/).
> - Enable new Menu UI when right clicking the Reload button. (Adds 'Normal Reload', 'Hard Reload', and 'Clear Cache and Hard Reload')
> - Home button, Chrome Labs button, and Bookmarks bar shown by default.
> - [Prefetch](https://www.chromium.org/developers/design-documents/dns-prefetching/) settings updated to respect privacy.
> - Patches for [GN](https://chromium.googlesource.com/chromium/src/tools/gn/+/48062805e19b4697c5fbd926dc649c78b6aaa138/README.md) and [chrome_sandbox](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/design/sandbox.md) when building.
> - Remove the addition of the Chrome APT sources.list during installation.
> - Widevine CDM Patch for Linux.
> - GTK auto dark mode patch
> - Various new flags either developed from scratch, or added from Ungoogled Chromium. See [PATCHES.md](https://github.com/Alex313031/thorium/blob/main/docs/PATCHES.md)
> - Enable Parallel Downloading by Default.
> - Inclusion of <img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/pak.png" width="16"> [pak](https://github.com/Alex313031/thorium/tree/main/pak_src#readme) a utility for packing and unpacking the [&#42;.pak](https://textslashplain.com/2022/05/03/chromium-internals-pak-files/) files in Thorium or any other Chromium based browser.
> - Logo and Branding/Naming changed to the Thorium logo, Thorium name, and "Alex313031" being appended to "The Chromium Authors" in credits, etc.
> - .desktop file includes useful cmdline flags that enable experimental or useful features. (See [PATCHES.md](https://github.com/Alex313031/thorium/blob/main/docs/PATCHES.md))
> - Includes installer patches and files to include [ChromeDriver](https://chromedriver.chromium.org/home) and *thorium_shell* [(content_shell)](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/testing/web_tests_in_content_shell.md), with a .desktop file being provided for thorium_shell (named thorium-shell.desktop and shows in desktop environments as Thorium Content Shell). These are also included in the Windows releases, but it doesn't make a shorcut, although a .png and .ico is in the install directory for you to make your own shortcut with an icon. You can also run content_shell with the command thorium-shell (custom wrapper for it, located in /usr/bin/). You can run ChromeDriver at /usr/bin/chromedriver or chromedriver.exe on Windows. Also, patches for abseil library and mini_installer when building with AVX on Windows.
> - Right clicking the launcher after install gives three additional desktop actions, one to open thorium-shell, another to open in Safe Mode which disables any flags one has set in chrome://flags until the next launch, and lastly to open in Dark Mode which appends the --force-dark-mode flag.
- For more info, read the [PATCHES.md](https://github.com/Alex313031/thorium/blob/main/docs/PATCHES.md) file.
- Known bugs are in the [BUGS.md](https://github.com/Alex313031/thorium/blob/main/infra/BUGS.md) file.
- A list of Chromium command line flags can be found at > https://peter.sh/experiments/chromium-command-line-switches

## Building <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_light.svg#gh-dark-mode-only"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_dark.svg#gh-light-mode-only">
See > https://github.com/Alex313031/thorium/blob/main/docs/BUILDING.md

## Debugging <img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/bug.svg" width="28">
See > https://github.com/Alex313031/thorium/tree/main/infra/DEBUG#readme

-------
&nbsp;&minus; https://www.reddit.com/r/ChromiumBrowser/ is a subreddit I made for Thorium and general Thorium/Chromium discussion, https://thorium.rocks/ is the website I made for it, and https://alex313031.blogspot.com/ is a blog I made relating to Thorium/ThoriumOS. \
&nbsp;&minus; I also build ChromiumOS (now called ThoriumOS) with Thorium, Codecs, Widevine, linux-firmware/modules, and extra packages at > https://github.com/Alex313031/ChromiumOS/

&nbsp;&minus; Thanks to https://github.com/robrich999/ for some info and fixes that went into this project.\
&nbsp;&minus; Thanks to https://github.com/midzer/ for support and helping with builds. \
&nbsp;&minus; Also thanks to https://github.com/bromite/bromite, https://github.com/saiarcot895/chromium-ubuntu-build, https://github.com/Eloston/ungoogled-chromium, https://github.com/GrapheneOS/Vanadium, and https://github.com/iridium-browser/iridium-browser for patch code. \
&nbsp;&minus; The pak_src dir, and the binaries in *pack_src/bin* are credited to @freeer https://github.com/myfreeer/chrome-pak-customizer/

&nbsp;&nbsp; __NOTE:__ [*libpepflashplayer.so*](https://github.com/Alex313031/thorium/blob/main/infra/libpepflashplayer.so) is included for posterity and can be used to enable Adobe Flash on older Chromium releases. ʘ‿ʘ

*Thanks for using Thorium!*

<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/Thorium90_504.jpg" width="200">

<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/GitHub/GitHub-Mark-Light-32px.png#gh-dark-mode-only"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/GitHub/GitHub-Mark-32px.png#gh-light-mode-only">
