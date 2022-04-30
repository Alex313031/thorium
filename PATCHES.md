<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/Thorium90_502.jpg" width="200">

## List of patches included in Thorium

Compiler modifications include SSE4.2, AVX, AES and cflags, lto flags, and import_instr_limit flags set to /03, 3, and 100, respectively. \
NEW > Added some LLVM LOOP optimizations as -mllvm flags. See here > https://github.com/RobRich999/Chromium_Clang/issues/26#issuecomment-976883814

Fixed AVX Windows Builds as per > https://github.com/RobRich999/Chromium_Clang/issues/26#issuecomment-1003605112

.desktop files are created for Thorium itself and content-shell (named thorium-shell), with theses cmdline flags being appended to both >  '--enable-experimental-web-platform-features --new-canvas-2d-api --show-component-extension-options --autoplay-policy=user-gesture-required --enable-features=VaapiVideoDecoder,PageInfoAboutThisSite,ParallelDownloading' and with '--enable-clear-hevc-for-testing' also being appended to the thorium-shell.desktop file.

Ink > https://chromium.googlesource.com/chromium/src/third_party/+/c4f7a938fcdc796ad9fead3ab01e0508c129b831/ink NOTE: Disabled for now due to bugs!
  - Implemented with the args.gn flag "enable_ink = true" Adds PDF annotation features present in CrOS.

MPEG-DASH > https://en.wikipedia.org/wiki/Dynamic_Adaptive_Streaming_over_HTTP
  - Implemented with the args.gn flags "enable_hls_sample_aes = true" and "enable_mse_mpeg2ts_stream_parser = true"

Enable JPEG XL Image File Format by Default Patch
  - Made by me.

Enable Live Caption (SODA) on all builds Patch
  - Made by me.

Font Rendering Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/fix-font-rendering.patch
  - Fixes font rendering in certain instances.

VAAPI Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/enable-vaapi-on-linux.diff
  - Modified by me.

Intel HD Support Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/ihd-support.patch

Nicer Native Notifications Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/notifications-nicer

Title Bar Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/title-bar-default-system.patch-v35

VDPAU Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/vdpau-support.patch
  - Modified by me.

Audio Sandbox Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/fix-sandbox-for-audio.patch

DoH Secure Mode Patch > https://github.com/bromite/bromite/blob/master/build/patches/DoH-improvements.patch

DoH Bare Minimum HTTP Headers Patch > https://github.com/bromite/bromite/blob/master/build/patches/Reduce-HTTP-headers-in-DoH-requests-to-bare-minimum.patch

Enable Do Not Track By Default Patch > https://github.com/GrapheneOS/Vanadium/blob/12/patches/0045-enable-dubious-Do-Not-Track-feature-by-default.patch

Show full URLs by Default Patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/disable-formatting-in-omnibox.patch

Disable FLOC Patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/core/ungoogled-chromium/disable-floc.patch

Disable Google API Key Warning Patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/debian/disable/google-api-warning.patch & https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/inox-patchset/0013-disable-missing-key-warning.patch
  - Modified by me.

Search Engines Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/additional-search-engines.patch
  - Modified by me. Adds DuckDuckGo, Ecosia, Ask, and Yandex, with DuckDuckGo being set to "canonical" so that canonical gets a small amount of money when using it.

Always use the local NTP (New Tab Page) Patch > https://github.com/GrapheneOS/Vanadium/blob/12.1/patches/0052-always-use-local-new-tab-page.patch

Chromium Sandbox Patch (enables PIE) > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/inox-patchset/0016-chromium-sandbox-pie.patch
  - Increases security for the chrome-sandbox executable.

Fix Distilled Page Icons Patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/fix-distilled-icons.patch#L6
  - Fixes some issues with material design icons.

Enable Menu UI on Reload Button > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/enable-menu-on-reload-button.patch
  - Adds new right click menu to the reload button.

Show Home Button by Default Patch
  - Made by me.

Prefetch Privacy enabled by Default Patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/enable-default-prefetch-privacy-changes.patch

GN Respect number of --jobs patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/debian/gn/parallel.patch
  - Useful when building.

V8 SEGV Patch > https://github.com/FydeOS/chromium_os-raspberry_pi/blob/master/project-cros-pi/chromeos-base/chromeos-chrome/files/patches/001-workaround-for-v8-SEGV-issue.patch
  - Modified by me.

Widevine CDM CU Patch > https://launchpad.net/ubuntu/bionic/+source/chromium-browser

RPATH for libffmpeg.so Patch > https://launchpad.net/ubuntu/bionic/+source/chromium-browser

Disable the addition of the Google-Chrome sources.list in /etc/apt/sources.list.d/ during installation, and prevents apt from complaining about removing the nonexistent file during an apt remove --purge. - Created by me.

Installer patches to include unstripped and RPATH binaries, with chrome_sandbox (needed for older distros), chromedriver and content-shell being added along with an icon and .desktop file for content-shell. - Created by me.

Patches for mini_installer and abseil when using AVX on Windows. Credit goes to @RobRich999

The Thorium .desktop file and content-shell .desktop file have flags added for experimental and useful features, namely: Disabling the Default Browser check and InfoBar, Experimental web platform features, Experimental Canvas2D, showing component extensions on chrome://extensions, requiring a user gesture to initiate playback of media, VAAPI Video decoder, Page Info v2, and Parallel Downloading. The --enable-clear-hevc-for-testing in thorium-shell enables testing H.265 (but still won't demux it).
  - Modified by me.

- Any other code/patches was written from scratch by me.
