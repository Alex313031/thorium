<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/Thorium90_504.jpg" width="200">

## List of patches/changes/features included in Thorium <img src="https://raw.githubusercontent.com/Alex313031/thorium/main/logos/NEW/patches.png" width="32">

# Thorium Patch Inventory

This document tracks the current patch files under `other/`. The authoritative
application order is `patch_scripts/series/series`; this page mirrors that
series so reviewers can audit which patches are active and which subsystem
owns a change.

All active `other/**/*.patch` files must be represented in the series.
Conditional entries are applied only when the matching setup variant is
selected. Update this inventory whenever a patch is added, removed, renamed,
reordered, split, or assigned a new condition; do not infer application order
from this page.

## Known Origins

This table intentionally lists only patches with clear upstream, reference, or
compliance-significant origins. Patches not listed here are Thorium-maintained,
overlay-derived, or still need separate provenance review.

| Patch | Origin / reference |
| --- | --- |
| [`add-hevc-ffmpeg-decoder-parser.patch`](../other/add-hevc-ffmpeg-decoder-parser.patch) | [StaZhu/enable-chromium-hevc-hardware-decoding](https://github.com/StaZhu/enable-chromium-hevc-hardware-decoding) HEVC FFmpeg decoder/parser patch |
| [`add-ac3-eac3-ffmpeg-decoders.patch`](../other/add-ac3-eac3-ffmpeg-decoders.patch) | Thorium-maintained AC3/EAC3 FFmpeg decoder/parser/demuxer generated config overlay; applies after the HEVC FFmpeg decoder/parser patch and is relevant to bundled codec/license review |
| [`enable-hevc-ffmpeg-decoding.patch`](../other/enable-hevc-ffmpeg-decoding.patch) | [StaZhu/enable-chromium-hevc-hardware-decoding](https://github.com/StaZhu/enable-chromium-hevc-hardware-decoding) Chromium-side HEVC enablement |
| [`enable-mpeg2-ac3-eac3-decoding.patch`](../other/enable-mpeg2-ac3-eac3-decoding.patch) | MPEG2/AC3/EAC3 codec enablement derived from Chromium/Electron codec patch references, including [Muril-o/electron-chromium-codecs](https://github.com/Muril-o/electron-chromium-codecs); keep license review with FFmpeg changes |
| [`ffmpeg-branding.patch`](../other/ffmpeg-branding.patch) | Thorium FFmpeg branding/config patch; relevant to bundled codec/license review |
| [`widevine-cdm-support.patch`](../other/widevine-cdm-support.patch) | Widevine CDM integration and bundled-payload metadata; desktop payloads are synchronized from [StaZhu/enable-chromium-hevc-hardware-decoding](https://github.com/StaZhu/enable-chromium-hevc-hardware-decoding), ChromeOS payload provenance is tracked separately in the overlay, and redistribution requires separate Widevine license review |
| [`widevine-cdm-prebuilt-signatures.patch`](../other/widevine-cdm-prebuilt-signatures.patch) | Packages prebuilt Windows/macOS CDM signature payloads independently of genuine Widevine host verification |
| [`linux-widevine-cdm-locations.patch`](../other/linux-widevine-cdm-locations.patch) | Raspberry Pi / Linux Widevine location compatibility |
| [`raspi-netflix-chromeos-ua.patch`](../other/raspi-netflix-chromeos-ua.patch) | Raspberry Pi Netflix ChromeOS UA compatibility |
| [`llvm-optimized-toolchain-build.patch`](../other/llvm-optimized-toolchain-build.patch) | [RobRich999/Chromium_Clang](https://github.com/RobRich999/Chromium_Clang) LLVM optimized build-script work, adapted to use compatibility-safe tuning instead of an AVX2 ISA requirement |
| [`disable-vulkan-gpu-log-warnings.patch`](../other/disable-vulkan-gpu-log-warnings.patch) | [RobRich999/Chromium_Clang](https://github.com/RobRich999/Chromium_Clang) Linux patch reference |
| [`v8-remove-local-sse2-override.patch`](../other/v8-remove-local-sse2-override.patch) | Removes V8's local Win32 `/arch:SSE2` override so target objects inherit Thorium's canonical x86 profile |
| [`v8-linux-snapshot-toolchain.patch`](../other/v8-linux-snapshot-toolchain.patch) | Generates Windows and macOS ARM64 snapshots with the Linux x64 ARM64 simulator toolchain, aligns snapshot/runtime V8 control-flow integrity, and keeps native Linux AVX-512 snapshot generators on Thorium's baseline host toolchain |
| [`linux-disable-custom-titlebar-default.patch`](../other/linux-disable-custom-titlebar-default.patch) | [saiarcot895/chromium-ubuntu-build](https://github.com/saiarcot895/chromium-ubuntu-build) title-bar default system patch reference |
| [`content-gpu-vaapi-libva-config.patch`](../other/content-gpu-vaapi-libva-config.patch) | Linux VAAPI/libva behavior adapted from Linux Chromium packaging patch references |
| [`thorium-sandbox-compat.patch`](../other/thorium-sandbox-compat.patch) | Chromium sandbox compatibility; related to ungoogled/inox sandbox PIE patch family |
| [`omnibox-search-engine-icon-branding.patch`](../other/omnibox-search-engine-icon-branding.patch) | [ungoogled-software/contrib](https://github.com/ungoogled-software/contrib) default search icon tweak |
| [`keep-doh-enabled-on-managed-devices.patch`](../other/keep-doh-enabled-on-managed-devices.patch) | [uazo/Cromite](https://github.com/uazo/cromite) keeps DoH available unless explicitly disabled by policy, including on managed devices and systems with parental controls |
| [`reduce-doh-request-headers.patch`](../other/reduce-doh-request-headers.patch) | [uazo/Cromite](https://github.com/uazo/cromite) reduce DoH request headers patch |
| [`disable-privacy-sandbox.patch`](../other/disable-privacy-sandbox.patch) | M154 residual advertising feature defaults and attestation preload/update removal |
| [`disable-fetching-field-trials.patch`](../other/disable-fetching-field-trials.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) / Bromite disable field-trials fetching patch |
| [`disable-encryption.patch`](../other/disable-encryption.patch) | [ungoogled-chromium-windows](https://github.com/ungoogled-software/ungoogled-chromium-windows) portable encryption/machine-id patches plus Supermium portable-profile reversion commits |
| [`enable-saving-pages-from-all-schemes.patch`](../other/enable-saving-pages-from-all-schemes.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) enable page saving on more schemes patch |
| [`add-flag-for-close-confirmation.patch`](../other/add-flag-for-close-confirmation.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) close confirmation flag patch |
| [`disable-qr-code-generator.patch`](../other/disable-qr-code-generator.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) QR generator flag, adapted to preserve Chromium's `QRCodeGeneratorEnabled` enterprise-policy authority |
| [`add-flag-to-close-window-with-last-tab.patch`](../other/add-flag-to-close-window-with-last-tab.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) close-window-with-last-tab flag patch, adapted to Thorium behavior |
| [`add-flag-to-scroll-tabs.patch`](../other/add-flag-to-scroll-tabs.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) scroll-tabs flag patch |
| [`add-flag-for-double-click-close-tab.patch`](../other/add-flag-for-double-click-close-tab.patch) | Ported from Thorium 2024 UI patch; double-click closes tabs |
| [`add-flag-for-right-click-close-tab.patch`](../other/add-flag-for-right-click-close-tab.patch) | Ported from local `chromium-unstable` history; right-click closes tabs while Shift+right-click keeps the context menu |
| [`add-flag-for-hover-activate-tab.patch`](../other/add-flag-for-hover-activate-tab.patch) | Ported from local `chromium-unstable` history; hover-to-activate tab behavior |
| [`add-flag-for-open-bookmarks-in-new-tab.patch`](../other/add-flag-for-open-bookmarks-in-new-tab.patch) | Ported from local `chromium-unstable` history; bookmark foreground/background new-tab behavior |
| [`add-flag-for-open-omnibox-url-in-new-tab.patch`](../other/add-flag-for-open-omnibox-url-in-new-tab.patch) | Ported from local `chromium-unstable` history; omnibox foreground/background new-tab behavior |
| [`add-flag-for-incognito-themes.patch`](../other/add-flag-for-incognito-themes.patch) | Allow Incognito windows and web content to follow the browser theme color mode |
| [`add-flag-to-hide-extensions-menu.patch`](../other/add-flag-to-hide-extensions-menu.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) hide extensions menu flag patch, adapted to Thorium flags |
| [`add-flag-to-hide-tab-close-buttons.patch`](../other/add-flag-to-hide-tab-close-buttons.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) hide tab close buttons flag patch, adapted to Thorium flags |
| [`add-flag-for-custom-ntp.patch`](../other/add-flag-for-custom-ntp.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) custom NTP flag patch |
| [`add-flag-for-tab-hover-cards.patch`](../other/add-flag-for-tab-hover-cards.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) tab hover cards flag patch |
| [`add-flag-to-keep-all-history.patch`](../other/add-flag-to-keep-all-history.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) disable local history expiration flag patch |
| [`add-flag-to-clear-data-on-exit.patch`](../other/add-flag-to-clear-data-on-exit.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) clear-data-on-exit flag patch |
| [`disable-download-quarantine.patch`](../other/disable-download-quarantine.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) disable download quarantine patch |
| [`notify-shell-after-download-complete.patch`](../other/notify-shell-after-download-complete.patch) | Windows shell refresh notification after completed downloads |
| [`keep-expired-flags.patch`](../other/keep-expired-flags.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) keep expired flags patch |
| [`allow_manifest_v2_extensions.patch`](../other/allow_manifest_v2_extensions.patch) | [ungoogled-chromium](https://github.com/ungoogled-software/ungoogled-chromium) Manifest V2 extension support patch |
| [`android-extensions-support.patch`](../other/android-extensions-support.patch) | [uazo/Cromite](https://github.com/uazo/cromite) experimental Android extensions support patch |
| [`android-extensions-opt-in-ui.patch`](../other/android-extensions-opt-in-ui.patch) | Thorium Android extension opt-in instructions and disabled-state UI |
| [`enable-extension-in-incognito.patch`](../other/enable-extension-in-incognito.patch) | [uazo/Cromite](https://github.com/uazo/cromite) enable extension in incognito patch |
| [`GPC.patch`](../other/GPC.patch) | Global Privacy Control behavior adapted from privacy-focused Chromium patch references |
| [`disable-middle-click-paste-with-autoscroll.patch`](../other/disable-middle-click-paste-with-autoscroll.patch) | Thorium-maintained Linux behavior fix for disabling selection clipboard paste paths when Blink MiddleClickAutoscroll is enabled |
| [`mini_installer.patch`](../other/mini_installer.patch) | Thorium Windows mini_installer GUI, SIMD-aware naming and packaging manifest; baseline installer targets disable shared C++ modules to avoid ISA mismatches |

## Series Patches


### 05 - Child repositories and generated third_party data.

- [`add-hevc-ffmpeg-decoder-parser.patch`](../other/add-hevc-ffmpeg-decoder-parser.patch) (apply root: `third_party/ffmpeg`)
- [`add-ac3-eac3-ffmpeg-decoders.patch`](../other/add-ac3-eac3-ffmpeg-decoders.patch) (apply root: `third_party/ffmpeg`)
- [`fix-ffmpeg-android-x86-disable-hevc-nasm.patch`](../other/fix-ffmpeg-android-x86-disable-hevc-nasm.patch) (apply root: `third_party/ffmpeg`)
- [`ffmpeg-branding.patch`](../other/ffmpeg-branding.patch) (apply root: `third_party/ffmpeg`)
- [`widevine-cdm-support.patch`](../other/widevine-cdm-support.patch) (apply root: `third_party/widevine`)
- [`thorium-search-engines-data.patch`](../other/thorium-search-engines-data.patch) (apply root: `third_party/search_engines_data/resources`)
- [`zstd-clang-cl-bmi2-detection.patch`](../other/zstd-clang-cl-bmi2-detection.patch) (apply root: `third_party/zstd/src`) - Keeps clang-cl AVX2 builds on zstd's runtime BMI2 dispatch unless `-mbmi2` is explicitly enabled.
- [`v8-remove-local-sse2-override.patch`](../other/v8-remove-local-sse2-override.patch) (apply root: `v8`)
- [`v8-linux-snapshot-toolchain.patch`](../other/v8-linux-snapshot-toolchain.patch) (apply root: `v8`)
- [`angle-lockfree.patch`](../other/SSE2/angle-lockfree.patch) (apply root: `third_party/angle/src`; condition: `sse2`) - Fails the build if `std::atomic<uint64_t>` is not lock-free, which on 32-bit x86 needs an i586 or newer baseline for `cmpxchg8b`.

### 10 - Media, codecs, and third_party-facing browser glue.

- [`enable-hevc-ffmpeg-decoding.patch`](../other/enable-hevc-ffmpeg-decoding.patch)
- [`enable-webrtc-h265-l1t2-l1t3-by-default.patch`](../other/enable-webrtc-h265-l1t2-l1t3-by-default.patch)
- [`enable-mpeg2-ac3-eac3-decoding.patch`](../other/enable-mpeg2-ac3-eac3-decoding.patch)
- [`thorium-media-switches.patch`](../other/thorium-media-switches.patch)
- [`widevine-cdm-prebuilt-signatures.patch`](../other/widevine-cdm-prebuilt-signatures.patch)

### 20 - Product defaults, identity, resources, branding, and browser preferences.

- [`thorium-default-api-keys.patch`](../other/thorium-default-api-keys.patch)
- [`disable-fetching-field-trials.patch`](../other/disable-fetching-field-trials.patch)
- [`thorium-blink-feature-defaults.patch`](../other/thorium-blink-feature-defaults.patch)
- [`allow-webaudio-autoplay.patch`](../other/allow-webaudio-autoplay.patch)
- [`enable-saving-pages-from-all-schemes.patch`](../other/enable-saving-pages-from-all-schemes.patch)
- [`content-gpu-vaapi-libva-config.patch`](../other/content-gpu-vaapi-libva-config.patch)
- [`thorium-content-shell-branding.patch`](../other/thorium-content-shell-branding.patch)
- [`thorium-common-branding-paths.patch`](../other/thorium-common-branding-paths.patch)
- [`android-thorium-branding.patch`](../other/android-thorium-branding.patch)
- [`thorium-startup-logging.patch`](../other/thorium-startup-logging.patch)
- [`thorium-app-metadata-branding.patch`](../other/thorium-app-metadata-branding.patch)
- [`thorium-theme-resources.patch`](../other/thorium-theme-resources.patch) — Registers all eight Thorium WebUI images in M154’s theme resource map, retaining Android’s direct resource-ID access to the version-page mascot.
- [`thorium-app-vector-icons.patch`](../other/thorium-app-vector-icons.patch)
- [`preinstall-ublock-origin.patch`](../other/preinstall-ublock-origin.patch) — Bundles verified uBlock Origin 1.75.0 for offline installation in new profiles, retaining upstream's update URL.
- [`bookmark-default-prefs.patch`](../other/bookmark-default-prefs.patch)
- [`bookmark-dialog-default-folder.patch`](../other/bookmark-dialog-default-folder.patch)
- [`increase-bookmark-open-prompt-threshold.patch`](../other/increase-bookmark-open-prompt-threshold.patch)
- [`thorium-browser-ui-default-prefs.patch`](../other/thorium-browser-ui-default-prefs.patch)
- [`tab-strip-combo-button-position.patch`](../other/tab-strip-combo-button-position.patch) — Keeps Chromium's unified horizontal tab-strip combo button on the physical right by default and provides a Thorium flag for physical-left alignment, including in RTL layouts.
- [`dom-distiller-reader-mode.patch`](../other/dom-distiller-reader-mode.patch)

### 30 - Build graph, toolchain, and platform build behavior.

- [`thorium-root-build-targets.patch`](../other/thorium-root-build-targets.patch)
- [`thorium-chrome-build-targets.patch`](../other/thorium-chrome-build-targets.patch)
- [`thorium-build-config-and-simd.patch`](../other/thorium-build-config-and-simd.patch)
- [`enable-xnnpack-arm-fp16-vector.patch`](../other/enable-xnnpack-arm-fp16-vector.patch)
- [`thorium-build-platform-tools.patch`](../other/thorium-build-platform-tools.patch) — Adds Linux build dependencies and adapts Windows import reordering to `thorium.exe`; uses upstream Windows toolchain selection and Apple linker argument handling.
- [`llvm-optimized-toolchain-build.patch`](../other/llvm-optimized-toolchain-build.patch)

### 40 - Linux/platform integration, UI debug tools, and shared resource branding.

- [`linux-disable-custom-titlebar-default.patch`](../other/linux-disable-custom-titlebar-default.patch)
- [`linux-obsolete-system-policy.patch`](../other/linux-obsolete-system-policy.patch)
- [`linux-memory-details-branding.patch`](../other/linux-memory-details-branding.patch)
- [`linux-shell-integration-branding.patch`](../other/linux-shell-integration-branding.patch)
- [`thorium-linux-installer-packaging.patch`](../other/thorium-linux-installer-packaging.patch)
  Preserves Thorium's stable DEB/RPM packaging; Flatpak is unsupported and explicitly disabled.
- [`thorium-ui-debug-shell.patch`](../other/thorium-ui-debug-shell.patch)
- [`thorium-webui-image-resources.patch`](../other/thorium-webui-image-resources.patch)
  Legacy WebUI icon overlays use `_old.svg`; corresponding unsuffixed icons retain Chromium's rounded designs.
- [`thorium-browser-resource-branding.patch`](../other/thorium-browser-resource-branding.patch)

### 50 - UI defaults, flags, WebUI, policy, FTP, and broad UI restoration.

- [`omnibox-search-engine-icon-branding.patch`](../other/omnibox-search-engine-icon-branding.patch)
- [`relax-bad-flags-warning.patch`](../other/relax-bad-flags-warning.patch)
- [`disable-startup-warning-infobars.patch`](../other/disable-startup-warning-infobars.patch)
- [`disable-default-browser-prompt.patch`](../other/disable-default-browser-prompt.patch)
- [`thorium-chrome-urls-page.patch`](../other/thorium-chrome-urls-page.patch)
- [`thorium-flags-registration.patch`](../other/thorium-flags-registration.patch)
- [`thorium-internal-url-scheme-alias.patch`](../other/thorium-internal-url-scheme-alias.patch) — Resolves `thorium://` to canonical internal URLs before Chromium's normal handlers, without granting the alias additional scheme privileges.
- [`thorium-internal-url-scheme-branding.patch`](../other/thorium-internal-url-scheme-branding.patch) — Displays `thorium://` in desktop and Android UI while retaining canonical internal URLs. Enabled by default and controlled by a flag.
- [`thorium-flags-page-branding.patch`](../other/thorium-flags-page-branding.patch)
- [`thorium-version-page-branding.patch`](../other/thorium-version-page-branding.patch)
- [`thorium-vector-icons.patch`](../other/thorium-vector-icons.patch)
- [`thorium-views-vector-icons.patch`](../other/thorium-views-vector-icons.patch) — Registers Thorium-specific Views icons; runtime fallback is owned by `disable-thorium-icons.patch`.
- [`thorium-app-menu-icons.patch`](../other/thorium-app-menu-icons.patch)
- [`prevent-url-elisions-by-default.patch`](../other/prevent-url-elisions-by-default.patch)
- [`disable-ai-entrypoints-by-default.patch`](../other/disable-ai-entrypoints-by-default.patch)
- [`enable-chrome-labs-by-default.patch`](../other/enable-chrome-labs-by-default.patch)
- [`enable-whats-new-by-default.patch`](../other/enable-whats-new-by-default.patch)
- [`fix-policy-templates.patch`](../other/fix-policy-templates.patch)
- [`ftp-support-thorium.patch`](../other/ftp-support-thorium.patch)
- [`GPC.patch`](../other/GPC.patch)
  Keeps Thorium's default-enabled `enable_gpc` preference and settings UI, using M154's shared GPC predicate for headers and Navigator APIs, including workers. The API remains exposed when disabled and reports the effective signal state.
- [`add-flag-for-close-confirmation.patch`](../other/add-flag-for-close-confirmation.patch)
- [`thorium-debug-mode.patch`](../other/thorium-debug-mode.patch)
- [`thorium-first-run-welcome.patch`](../other/thorium-first-run-welcome.patch)
- [`thorium-first-run-branding.patch`](../other/thorium-first-run-branding.patch) — Reuses the Thorium product SVG over the embedded Chromium logo in the refreshed first-run illustration shared by Windows, macOS, and Linux.

  Android's first-run sign-in animation uses the matching black inner ring via
  the [`signin_spin.json` resource overlay](../src/components/browser_ui/styles/android/java/res_chromium/raw/signin_spin.json).
- [`disable-middle-click-paste-with-autoscroll.patch`](../other/disable-middle-click-paste-with-autoscroll.patch) - Disables Linux middle-click selection paste and paste-and-navigate paths when Blink MiddleClickAutoscroll is enabled.
- [`views-menu-delay.patch`](../other/views-menu-delay.patch) - Adds `chrome://flags/#instant-menu-show` (disabled by default).
  Sets submenu delay to 1 ms; on Windows also fixes bookmark drag-hover at 400 ms.
  Default uses Chromium/platform timing. Independent of Th24; restart required.
  See [#1303](https://github.com/Alex313031/thorium/issues/1303).
- [`new-tab-to-left-context-menu.patch`](../other/new-tab-to-left-context-menu.patch)
- [`status-bubble-instant-expand.patch`](../other/status-bubble-instant-expand.patch)
- [`enable-sharing-hub-page-action.patch`](../other/enable-sharing-hub-page-action.patch) - Makes the Sharing Hub address-bar action visible by default in eligible normal windows using Chromium's current Page Action framework, while preserving policy, profile, window-mode, platform-icon, metrics, and bubble-anchor behavior.
- [`thorium-2024-ui-core.patch`](../other/thorium-2024-ui-core.patch) - Defines the optional Thorium 2024 feature and its native UI dimensions, colors, shapes, and flag-gated Settings WebUI styling. Sharing Hub enablement, menu interactions, and standalone command-line styles remain in dedicated patches.
- [`thorium-2024-menu-interactions.patch`](../other/thorium-2024-menu-interactions.patch) - Preserves the Th24-gated macOS menu navigation/accelerator behavior and Windows mnemonic visibility outside the core styling patch.
- [`custom-tab-width.patch`](../other/custom-tab-width.patch)
- [`expand-chrome-color-choices.patch`](../other/expand-chrome-color-choices.patch)
- [`classic-omnibox.patch`](../other/classic-omnibox.patch)
- [`rectangular-tabs.patch`](../other/rectangular-tabs.patch)
- [`prominent-active-tab-titles.patch`](../other/prominent-active-tab-titles.patch)
- [`toolbar-avatar-button-visibility.patch`](../other/toolbar-avatar-button-visibility.patch)
- [`restore-tab-button.patch`](../other/restore-tab-button.patch)
- [`thorium-chrome-labs-experiments.patch`](../other/thorium-chrome-labs-experiments.patch)
- [`add-flag-for-system-linux-theme.patch`](../other/add-flag-for-system-linux-theme.patch)
- [`restore_download_shelf.patch`](../other/restore_download_shelf.patch) — Restores the desktop download shelf; ChromeOS retains system download notifications and does not instantiate the shelf.
- [`downloads-page-enhancements.patch`](../other/downloads-page-enhancements.patch) — Shows full download URLs by default, controlled by `#downloads-show-full-url` on desktop. Retains Thorium's button layout and visible more-actions menu. Addresses [#1309](https://github.com/Alex313031/thorium/issues/1309).

### 60 - Installer, shell integration, and context-menu behavior.

- [`mini_installer.patch`](../other/mini_installer.patch) - Keeps the Thorium installer UI and packaging. System-level and same-version installs automatically close the target installation before file replacement. Uses Windows Restart Manager to let parent browsers save their session-exit state, then forcibly terminates remaining processes after bounded retries. Matches the executable path, keeps process handles, and registers PID/start-time pairs. Forced fallback can still require crash recovery; profile files and recovery UI are not altered. Inspection or termination failures remain installation errors. User-level cross-version installs retain Chromium's in-use update handling.
- [`open_in_same_tab.patch`](../other/open_in_same_tab.patch)
- [`windows-thorium-flags-conf.patch`](../other/windows-thorium-flags-conf.patch) - reads `thorium-flags.conf` from the install directory and `%AppData%\Thorium`, avoiding the removable `%LocalAppData%\Thorium` profile root during uninstall.

### 70 - User-facing flags, defaults, download/security behavior, and ThoriumOS.

- [`add-flag-to-close-window-with-last-tab.patch`](../other/add-flag-to-close-window-with-last-tab.patch)
- [`add-flag-to-scroll-tabs.patch`](../other/add-flag-to-scroll-tabs.patch)
- [`add-flag-for-double-click-close-tab.patch`](../other/add-flag-for-double-click-close-tab.patch)
- [`add-flag-for-right-click-close-tab.patch`](../other/add-flag-for-right-click-close-tab.patch)
- [`add-flag-for-hover-activate-tab.patch`](../other/add-flag-for-hover-activate-tab.patch)
- [`add-flag-for-open-bookmarks-in-new-tab.patch`](../other/add-flag-for-open-bookmarks-in-new-tab.patch)
- [`multi-column-bookmarks.patch`](../other/multi-column-bookmarks.patch) - Adds
  `chrome://flags/#multi-column-bookmarks` (default off; restart required) for
  Views bookmark menus. Columns fit the display work area, with vertical
  scrolling, keyboard navigation, drag-and-drop and live updates. Native macOS
  menus and specialized or window-manager-positioned menus keep their existing
  layout.
- [`add-flag-for-open-omnibox-url-in-new-tab.patch`](../other/add-flag-for-open-omnibox-url-in-new-tab.patch)
- [`disable-qr-code-generator.patch`](../other/disable-qr-code-generator.patch) — Adds a user-facing flag that disables QR-code sharing entry points while preserving Chromium's enterprise policy as an independent upper bound.
- [`hide-cast-context-menu.patch`](../other/hide-cast-context-menu.patch) — Adds a user-facing flag that hides Cast from rendered-page context menus without disabling Media Router or other Cast entry points.
- [`add-flag-for-incognito-themes.patch`](../other/add-flag-for-incognito-themes.patch)
- [`add-flag-to-hide-extensions-menu.patch`](../other/add-flag-to-hide-extensions-menu.patch)
- [`add-flag-to-hide-tab-close-buttons.patch`](../other/add-flag-to-hide-tab-close-buttons.patch)
- [`add-flag-for-custom-ntp.patch`](../other/add-flag-for-custom-ntp.patch)
- [`add-flag-for-tab-hover-cards.patch`](../other/add-flag-for-tab-hover-cards.patch)
- [`force-disable-tab-outlines.patch`](../other/force-disable-tab-outlines.patch)
- [`quiet-notification-defaults.patch`](../other/quiet-notification-defaults.patch)
- [`secure-doh-without-system-dns-config.patch`](../other/secure-doh-without-system-dns-config.patch) — Keeps explicitly configured Secure DoH available without system DNS configuration, with a `#disable-thorium-dns-config` opt-out. Other DNS modes and configuration merging retain upstream behavior.
- [`keep-doh-enabled-on-managed-devices.patch`](../other/keep-doh-enabled-on-managed-devices.patch) — Prevents device-management and parental-control detection from implicitly disabling DoH; an explicit managed `DnsOverHttpsMode` policy remains authoritative.
- [`add-flag-for-encrypted-client-hello.patch`](../other/add-flag-for-encrypted-client-hello.patch)
- [`reduce-doh-request-headers.patch`](../other/reduce-doh-request-headers.patch)
- [`disable-alternate-error-pages-by-default.patch`](../other/disable-alternate-error-pages-by-default.patch)
- [`add-flag-to-keep-all-history.patch`](../other/add-flag-to-keep-all-history.patch)
- [`add-flag-to-clear-data-on-exit.patch`](../other/add-flag-to-clear-data-on-exit.patch)
- [`enable-parallel-downloading-by-default.patch`](../other/enable-parallel-downloading-by-default.patch)
- [`disable-background-mode-by-default.patch`](../other/disable-background-mode-by-default.patch)
- [`thorium-dino-game.patch`](../other/thorium-dino-game.patch)
- [`allow-insecure-downloads.patch`](../other/allow-insecure-downloads.patch)
- [`disable-download-quarantine.patch`](../other/disable-download-quarantine.patch)
- [`notify-shell-after-download-complete.patch`](../other/notify-shell-after-download-complete.patch)
- [`disable-vulkan-gpu-log-warnings.patch`](../other/disable-vulkan-gpu-log-warnings.patch)
- [`thorium-sandbox-compat.patch`](../other/thorium-sandbox-compat.patch)
- [`thoriumos-ash-vector-icons.patch`](../other/thoriumos-ash-vector-icons.patch)
- [`thoriumos-help-app-discovery.patch`](../other/thoriumos-help-app-discovery.patch)
- [`thoriumos-sample-system-web-app.patch`](../other/thoriumos-sample-system-web-app.patch) — Enables the sample system web app in official builds using M154’s `is_chromeos` guard, preserving trusted/untrusted TypeScript resource mappings.
- [`thoriumos-disable-stats-reporting.patch`](../other/thoriumos-disable-stats-reporting.patch)
- [`add-flag-for-auto-dark-mode.patch`](../other/add-flag-for-auto-dark-mode.patch)
- [`disable-thorium-icons.patch`](../other/disable-thorium-icons.patch) — Uses Thorium icons by default; `--disable-thorium-icons` restores Chromium icons across native UI, WebUI and the Incognito new-tab page ([#1307](https://github.com/Alex313031/thorium/issues/1307)).
- [`always-enable-reload-menu.patch`](../other/always-enable-reload-menu.patch)

### 80 - Extensions, privacy, install branding, and late platform overlays.

- [`allow_manifest_v2_extensions.patch`](../other/allow_manifest_v2_extensions.patch)
- [`increase-dnr-limits.patch`](../other/increase-dnr-limits.patch)
- [`show-hosted-apps-in-extensions.patch`](../other/show-hosted-apps-in-extensions.patch)
- [`thorium_webui.patch`](../other/thorium_webui.patch)
- [`keep-expired-flags.patch`](../other/keep-expired-flags.patch)
- [`disable-privacy-sandbox.patch`](../other/disable-privacy-sandbox.patch) — Disables iframe URNs and the M1 advertising API override, removes attestation preload/update support and defaults Android APK fallback loading to off. Preserves attestation enforcement and upstream Fenced Frames, RWS, CHIPS, FedCM and Storage Access compatibility. Feature overrides remain available; existing user data is retained.
- [`disable-encryption.patch`](../other/disable-encryption.patch)
- [`disable-feature-promos.patch`](../other/disable-feature-promos.patch)
- [`thorium-install-static-branding.patch`](../other/thorium-install-static-branding.patch)
- [`windows-chrome-proxy-branding.patch`](../other/windows-chrome-proxy-branding.patch)
- [`windows-profile-shortcut-icon-version.patch`](../other/windows-profile-shortcut-icon-version.patch)
- [`disable-aero.patch`](../other/disable-aero.patch)
- [`android-disable-signin-without-account-manager.patch`](../other/android-disable-signin-without-account-manager.patch)
- [`android-extensions-support.patch`](../other/android-extensions-support.patch) — Adapts experimental Android extensions to M158, retaining the extension toggle and mobile UI defaults. Depends on `enable-extension-in-incognito.patch` for shared toolbar bridges.
- [`enable-extension-in-incognito.patch`](../other/enable-extension-in-incognito.patch) — Adds Cromite-derived Android incognito extension support while preserving regular-tab management and navigation.
  Includes Cromite's [fold/unfold guards](https://github.com/uazo/cromite/commit/010ed3a42e57f049f9b1a3525edb5d44c5e48353), [lifecycle fixes](https://github.com/uazo/cromite/commit/69c411bff972cd6cd6a807c144dbfbc84698e3d5) and [toolbar recreation](https://github.com/uazo/cromite/commit/297bf3438d35a6bd3a024a29b3a82913da718fab), plus [Thorium shutdown guards](https://github.com/gz83/thorium/pull/1) and an [Android menu fallback](https://github.com/ungoogled-software/ungoogled-chromium-android/blob/9dd576cd7536589f58ee96965599bc8e9af10809/patches/Extensions/misc/extensions-menu-null-guard.patch). Preserves background hosts shared by incognito windows; excludes Cromite's always-incognito routing and policy overrides.
- [`add-quick-extension-toggle-menu.patch`](../other/add-quick-extension-toggle-menu.patch) - Adds a default-off, `chrome://flags`-controlled quick enable/disable section to the extensions menu. Keeps the menu open when disabling the last extension so it can be re-enabled.
- [`fix-extensions-menu-scrollbar.patch`](../other/fix-extensions-menu-scrollbar.patch) - Uses Windows overlay scrollbars in both extensions menus when OS settings allow auto-hiding, with correct rounded-corner clipping. Follows accessibility and theme changes; other platforms retain their existing scrollbars.

- [`enable-android-devtools-frontend.patch`](../other/enable-android-devtools-frontend.patch) — Ports [Cromite's Android DevTools frontend](https://github.com/uazo/cromite/blob/master/build/patches/Enable-Android-DevTools-Frontend.patch), adding DevTools, Task Manager, More tools and touch submenus on phones. Follows the Android extension patches.
- [`android-extensions-opt-in-ui.patch`](../other/android-extensions-opt-in-ui.patch) — Keeps Android extensions opt-in, showing localized enabling instructions and a disabled load button until enabled. Routes menu and toolbar entry points to that page; follows the extension and Incognito bridge patches.
- [`developer-crx-installation.patch`](../other/developer-crx-installation.patch) — Adds opt-in CRX download installation requiring extension developer mode, plus a file picker and Android document-URI support. Preserves Web Store proof, signatures, permission prompts and policy checks. Follows the Android opt-in UI patch; addresses [#1281](https://github.com/Alex313031/thorium/issues/1281).

### 90 - Windows GDI text rendering and native font fallbacks (default off).

- [`windows-gdi-font-rendering-skia.patch`](../other/windows-gdi-font-rendering-skia.patch) (apply root: `third_party/skia`) — Ports bart-oski's Skia GDI support to Chromium `158.0.8127.41`, preserving font metrics, variable fonts, TTC face selection and PDF subsetting. Validates font data and retains native fallbacks for unsupported fonts.
- [`windows-gdi-font-rendering.patch`](../other/windows-gdi-font-rendering.patch) — Adds default-off `chrome://flags/#force-gdi` / `--disable-direct-write` for Windows page and browser UI text, excluding PDF renderers. Retains native font fallbacks and corrects UI spacing across display scales.

Both patches derive from [bart-oski's pinned Chromium 157 patch](https://gist.githubusercontent.com/bart-oski/8ada98da488e2fdd64e9e7b79764668c/raw/74f698750e50c3c9585a87e70192d77e3f8346aa/barts-gdi-chrome-157.0.8103.15.patch), retaining its license notices. MacType must load into the drawing process; native fallbacks may still render some fonts. Enabling GDI removes win32k lockdown and the non-system-font restriction for non-PDF GDI renderers, relaxes their startup DLL signing, and disables browser dynamic-code blocking unless managed policy requires it. Delayed mitigations and token lowering remain.

### 95 - Conditional / platform-specific overlays that are still active.

- [`linux-widevine-cdm-locations.patch`](../other/linux-widevine-cdm-locations.patch) (condition: `raspi`)
- [`raspi-netflix-chromeos-ua.patch`](../other/raspi-netflix-chromeos-ua.patch) (condition: `raspi`)

## Maintenance Notes

- Add, remove, and reorder active patches in `patch_scripts/series/series` first.
- Keep patch files under `other/`; use an apply root in the series for child repositories such as `third_party/ffmpeg`, `third_party/widevine`, `third_party/abseil-cpp`, `third_party/search_engines_data/resources`, and `v8`.
- Use one `--condition` per run for mutually exclusive build variants such as `sse2` or `raspi`.
- After changing patch inventory, run `py -3 patch_scripts\series\apply_series.py --source-tree C:\src\chromium\src` to validate the ordered series without modifying the Chromium checkout.
