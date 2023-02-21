<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/Thorium90_504.jpg" width="200">

## List of patches/changes/features included in Thorium <img src="https://raw.githubusercontent.com/Alex313031/Thorium/main/logos/NEW/patches.png" width="32">

Compiler modifications include SSE4.2, AVX, AES, and CFLAGS, LTO flags, and import_instr_limit flags set to /03, 3, and 30, respectively. \
NEW > Added some LLVM LOOP optimizations as -mllvm flags. See here > https://github.com/RobRich999/Chromium_Clang/issues/26#issuecomment-976883814

Fixed AVX Windows Builds as per > https://github.com/RobRich999/Chromium_Clang/issues/26#issuecomment-1003605112

.desktop files are created for Thorium itself and content-shell (named thorium-shell), with theses cmdline flags being appended to both via the wrapper file > '--no-default-browser-check --use-gl=desktop --enable-experimental-web-platform-features --new-canvas-2d-api --autoplay-policy=user-gesture-required --enable-features=PageInfoAboutThisSiteMoreInfo'.

There are also additional desktop actions: when right clicking the Thorium launcher it gives three additional desktop actions, one to open thorium-shell, another to open in Safe Mode which disables any flags one has set in chrome://flags until the next launch, and lastly to open in Dark Mode which appends the --force-dark-mode flag.

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

VAAPI on Wayland Patch > https://github.com/ungoogled-software/ungoogled-chromium-archlinux/blob/master/ozone-add-va-api-support-to-wayland.patch
  - From Arch AUR

Use default search icon instead of search engine icon in omnibox patch > https://github.com/ungoogled-software/contrib/blob/master/tweaks/visual/default-search-icon.patch

Intel HD Support Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/ihd-support.patch

Nicer Native Notifications Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/notifications-nicer

Title Bar Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/title-bar-default-system.patch-v35

VDPAU Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/vdpau-support.patch
  - Modified by me.

Audio Sandbox Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/fix-audio-sandbox.patch

DoH Secure Mode Patch > https://github.com/bromite/bromite/blob/master/build/patches/DoH-improvements.patch

DoH Bare Minimum HTTP Headers Patch > https://github.com/bromite/bromite/blob/master/build/patches/Reduce-HTTP-headers-in-DoH-requests-to-bare-minimum.patch

Enable Do Not Track By Default Patch > https://github.com/GrapheneOS/Vanadium/blob/13/patches/0044-enable-dubious-Do-Not-Track-feature-by-default.patch

Show full URLs by Default Patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/disable-formatting-in-omnibox.patch - Modified by me.

Disable FLOC Patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/core/ungoogled-chromium/disable-floc.patch

Disable Google API Key Warning Infobar Patch > https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/debian/disable/google-api-warning.patch & https://github.com/Eloston/ungoogled-chromium/blob/master/patches/extra/inox-patchset/0013-disable-missing-key-warning.patch
  - Modified by me.

Disable Default Browser Infobar Patch
  - Made by me.

Search Engines Patch > https://github.com/saiarcot895/chromium-ubuntu-build/blob/master/debian/patches/additional-search-engines.patch
  - Modified by me. Adds DuckDuckGo, Brave Search, Ecosia, Ask, and Yandex, with DuckDuckGo being set to "canonical" so that canonical gets a small amount of money when using it.

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

V8 SEGV Patch > https://github.com/FydeOS/chromium_os-raspberry_pi/blob/84ad1a83388bd57a9f1bc318c3057b10eb3101d2/project-cros-pi/chromeos-base/chromeos-chrome/files/patches/001-workaround-for-v8-SEGV-issue.patch
  - Modified by me.

Widevine CDM CU Patch > https://launchpad.net/ubuntu/bionic/+source/chromium-browser

Enable Parallel Downloading by Default Patch
  - Made by me.

RPATH for libffmpeg.so Patch > https://launchpad.net/ubuntu/bionic/+source/chromium-browser

Always Show Component Extensions Patch > https://github.com/iridium-browser/iridium-browser/commit/f8e1b5f19862867b2736ca0a25f6979fcc2d7d71
  - Modified by me.

Increase default key length for newly-generated RSA keys from 1024 to 2048 Patch > https://github.com/iridium-browser/iridium-browser/commit/d016769081706d591188b5b2929c5fc2efd8ef20

Enable UI Features: Side Search, Side Panel Journeys, Chrome Labs, Extensions Access Menu, Tab Hover Cards, Tab Outlines in Low Contrast Themes, More Prominent Active Tab Title in Dark Mode, WebUI Tab Strip, Drag and Drop Tabs on Wayland, Tab Groups Saving > https://github.com/Alex313031/Thorium/blob/main/chrome/browser/ui/ui_features.cc
  - Made by me.

Enable Precompiling of Inline Scripts in HTML
  - Modified by me.

Patch Windows installer to close previous browser instance > https://github.com/Hibbiki/chromium-win64/blob/master/patch/0001-Patch-installer-to-close-previous-browser-instance-o.patch

Disable the addition of the Google-Chrome sources.list in /etc/apt/sources.list.d/ during installation, and prevents apt from complaining about removing the nonexistent file during an apt remove --purge. - Created by me.

Enable Dark Mode flag Patch
  - Made by me.

Disable AutoPlay flag Patch
  - Made by me.

Enable Saving pages from all Schemes Patch > https://github.com/ungoogled-software/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/enable-page-saving-on-more-pages.patch

Allow setting a custom NTP flag Patch > https://github.com/ungoogled-software/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/add-flag-for-custom-ntp.patch

Allow removing the Side Panel button flag Patch > https://github.com/ungoogled-software/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/add-flag-to-hide-side-panel-button.patch

Unexpire Flags by Default Patch > https://github.com/ungoogled-software/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/keep-expired-flags.patch
  - Modified by me.

Disable Download Quarantine Patch > https://github.com/ungoogled-software/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/disable-download-quarantine.patch

Show/Hide Avatar Button Patch > https://github.com/ungoogled-software/ungoogled-chromium/blob/master/patches/extra/ungoogled-chromium/add-flag-to-show-avatar-button.patch

Enable Reader Mode Patch - Made by me.

Disable WebGL2, WebGL Anti-Aliasing Mode, WebGL MSAA Sample Count, and Native GPU Rasterization MSAA Sample Count chrome://flags Patches - Made by me.

Number of Raster Threads chrome://flags Patch - Made by me.

Enable Native UI Inspection in DevTools chrome://flags Patch - Made by me.

Enable High Contrast mode Patch - Made by me.

Enable Local File Accesses, Allow File URI Access from Files, and Disable Web Security chrome://flags Patches - Made by me.

Enable HEVC/H.265 Decoding Patch - https://github.com/StaZhu/enable-chromium-hevc-hardware-decoding/blob/main/add-hevc-ffmpeg-decoder-parser.patch
  - Modified by me.

Allow all HEVC Video Profiles to Play Patch - https://github.com/StaZhu/enable-chromium-hevc-hardware-decoding/blob/main/remove-main-main10-profile-limit.patch

Enable AC3 and EAC3 for HEVC Patch - https://github.com/Muril-o/electron-chromium-codecs

Show the Apps button in Bookmarks Bar by Default Patch - Made by me.

Add autocomplete for chrome://flags > https://github.com/bromite/bromite/blob/master/build/patches/Offer-builtin-autocomplete-for-chrome-flags.patch

Disable fetching Field Trials/Variations Patch - https://github.com/ungoogled-software/ungoogled-chromium/blob/master/patches/core/bromite/disable-fetching-field-trials.patch

Enable double click to close tab flag - https://github.com/bigfoxtail/brave-core/commit/acec5efcbaa98722611f551712f051fb343af120
  - Found by @gz83, modified by me.

Installer patches to include unstripped and RPATH binaries, with chrome_sandbox (needed for older distros), chromedriver and content-shell being added along with an icon and .desktop file for content-shell. - Created by me.

Patches for mini_installer and abseil when using AVX on Windows. Credit goes to @RobRich999

The Thorium .desktop file and content-shell .desktop file have flags added for experimental and useful features, namely: Disabling the Default Browser check and InfoBar, Experimental web platform features, Experimental Canvas2D, requiring a user gesture to initiate playback of media, VAAPI Video decoder, Page Info v2.
  - Modified by me.

- Any other code/patches are written from scratch by me.
