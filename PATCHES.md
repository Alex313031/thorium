## List of patches included in Thorium

Compiler modifications include AVX, FMA, and cflags, lto flags, and import_instr_limit flags set to /03, 3, and 30, respectively. \
NEW > Added some LLVM Loop optimizations as ldflags and cflags. See here > https://github.com/RobRich999/Chromium_Clang/issues/26#issuecomment-976883814

.desktop files are created for Thorium itself and content-shell (named thorium-shell), with theses cmdline flags being appended to both >  '--enable-experimental-web-platform-features --new-canvas-2d-api --show-component-extension-options --autoplay-policy=user-gesture-required --enable-features=VaapiVideoDecoder,PageInfoAboutThisSite,ParallelDownloading' and with '--enable-clear-hevc-for-testing' also being appended to the thorium-shell.desktop file.

Ink > https://chromium.googlesource.com/chromium/src/third_party/+/c4f7a938fcdc796ad9fead3ab01e0508c129b831/ink
  - Implemented with the flag "enable_ink = true"

MPEG-DASH > https://en.wikipedia.org/wiki/Dynamic_Adaptive_Streaming_over_HTTP
  - Implemented with the flag "enable_hls_sample_aes = true" and "enable_mse_mpeg2ts_stream_parser = true"

Font Rendering Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/fix-font-rendering.patch

VAAPI Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/enable-vaapi-on-linux.diff

Nicer Native Notifications Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/notifications-nicer

Title Bar Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/title-bar-default-system.patch-v35

VDPAU Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/vdpau-support.patch
  - Modified by me.

Audio Sandbox Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/fix-sandbox-for-audio.patch

DoH Secure Mode Patch > https://github.com/bromite/bromite/blob/master/build/patches/DoH-secure-mode-by-default.patch

DoH Bare Minimum HTTP Headers Patch > https://github.com/bromite/bromite/blob/master/build/patches/Reduce-HTTP-headers-in-DoH-requests-to-bare-minimum.patch

Enable Do Not Track By Default Patch > https://github.com/GrapheneOS/Vanadium/blob/12/patches/0045-enable-dubious-Do-Not-Track-feature-by-default.patch

Disable FLOC Patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/core/ungoogled-chromium/disable-floc.patch

Disable Google API Key Warning Patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/debian/disable/google-api-warning.patch

Search Engines Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/additional-search-engines.patch
  - Modified by me.

Installer patches to include unstripped and RPATH binaries, with chrome_sandbox (needed for older distros), chromedriver and content-shell being added along with an icon and .desktop file for content-shell.
The Thorium .desktop file and content-shell .desktop file have flags added for experimental and useful features, namely: Experimental web platform features, Experimental Canvas2D, showing component extensions on chrome://extensions, requiring user gesture to initiate playback of media, VAAPI Video decoder, Page Info v2, and Parallel Downloading. The --enable-clear-hevc-for-testing in thorium-shell enables testing H.265 (but still won't demux it).
  - Modified by me.

- Any other code/patches was written from scratch by me.


