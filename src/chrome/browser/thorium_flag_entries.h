// Copyright (c) 2026 Alex313031.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_THORIUM_FLAG_ENTRIES_H_
#define CHROME_BROWSER_THORIUM_FLAG_ENTRIES_H_

    {"force-dark-mode",
     "Enable Dark Mode",
     "Enables dark mode for all UI elements (but not web contents - turn on #enable-force-dark for darkening web contents).",
     kOsDesktop, SINGLE_VALUE_TYPE(switches::kForceDarkMode)},

#if BUILDFLAG(IS_LINUX)
    {"auto-dark-mode",
     "GTK Auto Dark Mode",
     "Enables Thorium to automatically change to Dark Mode according to the system GTK Theme.",
     kOsLinux, SINGLE_VALUE_TYPE("auto-dark-mode")},
    {"use-system-linux-theme",
     "Use GTK/Qt System Theme",
     "Allows Thorium to use the detected GTK or Qt system theme on Linux instead of the default Thorium 2024 UI theme behavior.",
     kOsLinux, SINGLE_VALUE_TYPE("use-system-linux-theme")},
#endif // BUILDFLAG(IS_LINUX)

    {"thorium-2024",
     "Enable Experimental Thorium 2024 (Th24) UI",
     "Enable a new \"hybrid\" UI, which restores many parts of the pre-Chrome Refresh 2023 UI. Good for people "
     "who find the new UI ugly or harder to use.",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kThorium2024)},
    {"instant-menu-show",
     "Instant Menu Show",
     "Reduces submenu hover delay to 1 ms. Disabled by default; when "
     "disabled, uses 400 ms or the Windows system setting. On Windows, "
     "enabling this also fixes bookmark drag-and-drop hover delay at 400 ms.",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kThoriumInstantMenus)},
    {"thorium-internal-url-scheme",
     "Use thorium:// for internal URLs",
     "Displays and copies internal browser URLs using the thorium:// alias. "
     "The canonical internal scheme remains chrome://, and both schemes are "
     "accepted.",
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(features::kThoriumInternalUrlSchemeBranding)},
    {"disable-qr-code-generator",
     "Disable QR Code Generator",
     "Disables the QR code generator menu entries used to share page links. "
     "The QRCodeGeneratorEnabled enterprise policy remains authoritative.",
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kThoriumDisableQRCodeGenerator)},
    {"hide-cast-context-menu",
     "Hide Cast Context Menu Item",
     "Hides the Cast menu item from page and media context menus without "
     "disabling Media Router or other Cast entry points.",
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kThoriumHideCastContextMenu)},
#if !BUILDFLAG(IS_ANDROID)
    {"left-aligned-tab-strip-combo-button",
     "Left-Aligned Tab Strip Combo Button",
     "Places the horizontal tab strip combo button on the physical left side "
     "of the tab strip. The combo button is on the right by default.",
     kOsDesktop,
     FEATURE_VALUE_TYPE(tabs::kThoriumLeftAlignedTabStripComboButton)},
#endif  // !BUILDFLAG(IS_ANDROID)
    {"restore-tab-button",
     "Restore Tab Button",
     "Enable a new toolbar button to restore your recently closed tabs.",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kRestoreTabButton)},
    {"prominent-active-tab-titles",
     "Prominent Active Tab Titles",
     "Makes the active tab title bolder so that it is easier to identify.",
     kOsDesktop, SINGLE_VALUE_TYPE("prominent-active-tab-titles")},
    {"force-disable-tab-outlines",
     "Disable Tab Outlines",
     "Force disables tab outline strokes. Thorium enables them by default, improving accessiblity in dark mode, incognito mode, and low contrast themes.",
     kOsDesktop, SINGLE_VALUE_TYPE("force-disable-tab-outlines")},
    {"disable-thorium-icons",
     "Disable Thorium Custom Icons",
     "Uses Chromium's default icon designs and colors instead of "
     "Thorium's custom icons throughout the browser UI.",
     kOsDesktop, SINGLE_VALUE_TYPE("disable-thorium-icons")},
    {"disable-chrome-labs",
     "Disable Chrome Labs",
     "Hides the Chrome Labs entry from the toolbar and app menu.",
     kOsDesktop, SINGLE_VALUE_TYPE("disable-chrome-labs")},
    {"hide-extensions-menu",
     "Hide Extensions Menu",
     "Hides the extensions container. This includes the puzzle piece icon as well as any pinned extensions.",
     kOsDesktop, SINGLE_VALUE_TYPE("hide-extensions-menu")},
#if BUILDFLAG(ENABLE_EXTENSIONS) && !BUILDFLAG(IS_ANDROID)
    {"extensions-menu-quick-toggles",
     "Extensions Menu Quick Toggles",
     "Shows a quick enable/disable section for installed extensions in the extensions menu.",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kExtensionsMenuQuickToggle)},
#endif  // BUILDFLAG(ENABLE_EXTENSIONS) && !BUILDFLAG(IS_ANDROID)
#if BUILDFLAG(ENABLE_EXTENSIONS_CORE)
    {"allow-offstore-extension-downloads",
     "Allow Third-Party Extension Downloads",
     "Allows downloaded CRX files to be installed when developer mode is enabled on the extensions page. CRX signatures, permission prompts, and extension policies still apply. Android also requires experimental extension support. Only enable this for sources you trust.",
     kOsAll, SINGLE_VALUE_TYPE("allow-offstore-extension-downloads")},
#endif  // BUILDFLAG(ENABLE_EXTENSIONS_CORE)
    {"classic-omnibox",
     "Classic Omnibox UI",
     "Changes the omnibox shape to be more square.",
     kOsDesktop, SINGLE_VALUE_TYPE("classic-omnibox")},
    {"rectangular-tabs",
     "Thorium Rectangular Tabs UI",
     "Changes the look of browser tabs to appear with a rectangular shape, similar to Vivaldi or Cent Browser.",
     kOsDesktop, SINGLE_VALUE_TYPE("rectangular-tabs")},
    {"ctrl-tab-mru",
     "Ctrl+Tab Switches to Most Recently Used Tab",
     "Makes Ctrl+Tab switch to the previously used tab instead of the next adjacent tab.",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kCtrlTabMru)},

    {"custom-tab-width",
     "Custom Tab Width",
     "Allows setting the default tab width, in DIP. Normally 1 DIP = 1 Pixel, and the standard width for tabs is 240.",
     kOsDesktop, MULTI_VALUE_TYPE(kCustomTabWidthChoices)},
    {"disable-thorium-dns-config",
     "Disable Thorium Secure DNS Fallback",
     "Disables Thorium's fallback that keeps explicitly configured Secure DNS-over-HTTPS available when the platform DNS configuration is unavailable. "
     "Automatic Secure DNS and normal system DNS configuration merging are not affected.",
     kOsAll, SINGLE_VALUE_TYPE("disable-thorium-dns-config")},
    {"encrypted-client-hello",
     "Encrypted ClientHello",
     "Controls whether Thorium allows TLS Encrypted ClientHello. Enabled still requires server support and usable HTTPS/SVCB DNS records.",
     kOsAll, MULTI_VALUE_TYPE(kEncryptedClientHelloChoices)},
    {"webrtc-ip-handling-policy",
     "WebRTC IP Handling Policy",
     "Controls which network interfaces WebRTC can use. Default follows your "
     "current settings; Thorium uses the default public and private interfaces "
     "unless overridden. Enterprise policies and extension settings take "
     "precedence over this flag. Restart required.",
     kOsAll, MULTI_VALUE_TYPE(kWebRtcIPHandlingPolicyChoices)},

#if !BUILDFLAG(IS_ANDROID)
    {"show-component-extension-options",
     "Show Component Extension Options",
     "Shows internal Chromium component extensions on the `chrome://extensions`. These are normally hidden, "
     "but this is an override for debugging or inspection.",
     kOsDesktop, SINGLE_VALUE_TYPE(extensions::switches::kShowComponentExtensionOptions)},
#endif // BUILDFLAG(IS_ANDROID)

    {"force-high-contrast",
     "Enable High Contrast Mode",
     "Enables high contrast mode for all Thorium instances.",
     kOsDesktop, SINGLE_VALUE_TYPE(switches::kForceHighContrast)},

#if BUILDFLAG(IS_WIN)
    {"disable-aero",
     "Disable Aero Window Frame Compositing",
     "Use the classic Chromium theme designed to mimick \"Aero\" window controls. "
     "Typically used when desktop composition is disabled or unavailable.",
     kOsWin, SINGLE_VALUE_TYPE("disable-aero")},
#endif // BUILDFLAG(IS_WIN)

    {"custom-ntp",
     "Custom New Tab Page",
     "Allows setting a custom URL for the New Tab Page (NTP). Value can be internal (e.g. `about:blank` or `chrome://new-tab-page`), "
     "external (e.g. `example.com`), or local (e.g. `file:///tmp/startpage.html`). "
     "This applies for incognito windows as well when not set to a `chrome://` internal page.",
     kOsDesktop, ORIGIN_LIST_VALUE_TYPE("custom-ntp", "")},
    {"scroll-tabs",
     "Scroll Switches Active Tab",
     "Switch to the left/right tab if a scroll wheel event happens over the tabstrip, or the empty space beside the tabstrip.",
     kOsDesktop, MULTI_VALUE_TYPE(kScrollEventChangesTab)},

#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
    {"middle-click-autoscroll",
     "Middle Click Autoscroll",
     "Enables autoscrolling when the middle mouse button is pressed.",
     kOsDesktop,
     ENABLE_DISABLE_VALUE_TYPE_AND_VALUE(switches::kEnableBlinkFeatures,
                                         "MiddleClickAutoscroll",
                                         switches::kDisableBlinkFeatures,
                                         "MiddleClickAutoscroll")},
#endif // BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)

    {"autoplay-policy",
     "Configure AutoPlay Policy",
     "Controls the autoplay policy for HTML audio and video elements. "
     "On desktop, Web Audio contexts bypass autoplay user-gesture checks "
     "regardless of this setting.",
     kOsDesktop, MULTI_VALUE_TYPE(kAutoplayPolicyChoices)},
    {"allow-insecure-downloads",
     "Allow Insecure Downloads",
     "Allows downloading files from mixed origin/cross origin schemes.",
     kOsAll, SINGLE_VALUE_TYPE("allow-insecure-downloads")},

#if !BUILDFLAG(IS_ANDROID)
    {"download-shelf",
     "Restore Download Shelf",
     "When enabled, the traditional download shelf is used instead of the download bubble in the toolbar. Thorium flag",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kDownloadShelf)},
    {"downloads-show-full-url",
     "Show Full Download URLs",
     "Shows full download URLs below the source on the downloads page. "
     "Enabled by default. Disable to show only the source, as in Chromium. "
     "The copy download link button remains available.",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kThoriumDownloadsShowFullUrl)},
#endif // BUILDFLAG(IS_ANDROID)

    {"show-avatar-button",
     "Show/Hide the Avatar Button",
     "Show the Avatar/People/Profile button in the browser toolbar: Always, Incognito|Guest only, or Never.",
     kOsDesktop, MULTI_VALUE_TYPE(kShowAvatarButtonChoices)},
    {"keep-all-history",
     "Keep All History",
     "Retain All local browsing history. By default history older than 4 months is expired and purged. Thorium flag",
     kOsAll, SINGLE_VALUE_TYPE("keep-all-history")},
    {"clear-data-on-exit",
     "Clear Data on Exit",
     "Clears browsing history, downloads, cache, site data, passwords, form data, and content settings on exit.",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kClearDataOnExit)},
    {"webgl-msaa-sample-count",
     "WebGL MSAA Sample Count",
     "Set a default sample count for WebGL if MSAA is enabled on the GPU.",
     kOsAll, MULTI_VALUE_TYPE(kWebglMSAASampleCountChoices)},
    {"webgl-antialiasing-mode",
     "WebGL Anti-Aliasing Mode",
     "Set the antialiasing method used for WebGL. (None, Explicit, Implicit)",
     kOsAll, MULTI_VALUE_TYPE(kWebglAntialiasingModeChoices)},
    {"gpu-rasterization-msaa-sample-count",
     "Native GPU Rasterization MSAA Sample Count",
     "Set a default sample count for native GPU Rasterization if MSAA is enabled on the GPU.",
     kOsAll, MULTI_VALUE_TYPE(kGpuRasterizationMSAASampleCountChoices)},
    {"num-raster-threads",
     "Number of Raster Threads",
     "Specify the number of worker threads used to rasterize content.",
     kOsAll, MULTI_VALUE_TYPE(kNumRasterThreadsChoices)},
    {"force-gpu-mem-available-mb",
     "Set GPU Available Memory",
     "Sets the total amount of memory (in MB) that may be allocated for GPU resources.",
     kOsDesktop, MULTI_VALUE_TYPE(kForceGpuMemAvailableMbChoices)},

#if BUILDFLAG(IS_LINUX)
    {"enable-native-gpu-memory-buffers",
     "Enable Native GPU Memory Buffers",
     "Enables native CPU-mappable GPU memory buffer support on Linux.",
     kOsLinux, SINGLE_VALUE_TYPE(switches::kEnableNativeGpuMemoryBuffers)},
    {"vaapi-video-decode-linux-gl",
     "GL Vaapi Video Decode",
     "Toggle whether the GL backend is used for VAAPI video decode acceleration. "
     "Enabled by default, but may break some configurations. Thorium flag.",
     kOsLinux, FEATURE_VALUE_TYPE(media::kAcceleratedVideoDecodeLinuxGL)},
    {"touchpad-overscroll-history-navigation",
     "Touchpad Overscroll History Navigation",
     "Enables back/forward navigation via touchpad overscroll gestures.",
     kOsLinux, FEATURE_VALUE_TYPE(features::kTouchpadOverscrollHistoryNavigation)},
    {"gtk-version",
     "GTK Version Override",
     "Choose whether to use the GTK3 or GTK4 backend. It should be set to match the default GTK used by the system, "
     "but can be overridden for testing or experimenting.",
     kOsLinux, MULTI_VALUE_TYPE(kGtkVersionChoices)},
    {"vaapi-on-nvidia-gpus",
     "VAAPI on nVidia GPUs",
     "Toggle whether VAAPI is enabled when proprietary nVidia Drivers are installed. "
     "Requires `vdpau-va-driver` to be installed, and can be buggy. Thorium flag.",
     kOsLinux, FEATURE_VALUE_TYPE(media::kVaapiOnNvidiaGPUs)},
#endif // BUILDFLAG(IS_LINUX)

    {"gpu-no-context-lost",
     "No GPU Context Lost",
     "Inform Thorium's GPU process that a GPU context will not be lost in power saving mode, screen saving mode, etc. "
     "Note that this flag does not ensure that a GPU context will never be lost in any situation, like say, a GPU reset. "
     "Useful for fixing blank or pink screens/videos upon system resume, etc.",
     kOsDesktop, SINGLE_VALUE_TYPE(switches::kGpuNoContextLost)},
    {"tab-hover-cards",
     "Tab Hover Cards",
     "Allows removing the tab hover cards or using a tooltip as a replacement.",
     kOsDesktop, MULTI_VALUE_TYPE(kTabHoverCardChoices)},
    {"hide-tab-close-buttons",
     "Hide Tab Close Buttons",
     "Hides the close buttons on tabs.",
     kOsDesktop, SINGLE_VALUE_TYPE("hide-tab-close-buttons")},
    {"double-click-close-tab",
     "Double Click to Close Tab",
     "Enables double clicking a tab to close it.",
     kOsDesktop, SINGLE_VALUE_TYPE("double-click-close-tab")},
    {"right-click-to-close-tab",
     "Right Click to Close Tab",
     "Enables closing tabs with a right click. Hold Shift while right clicking to show the tab context menu instead.",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kRightClickToCloseTab)},
    {"hover-activate-tab",
     "Hover to Activate Tab",
     "Activates a tab after hovering the mouse over it for the selected delay.",
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kHoverActivateTab,
                                    kHoverActivateTabVariations,
                                    "HoverActivateTab")},
    {"open-bookmarks-in-new-tab",
     "Open Bookmarks in New Tab",
     "Opens single bookmark clicks in a foreground or background new tab instead of the current tab.",
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kOpenBookmarksInNewTab,
                                    kOpenBookmarksInNewTabVariations,
                                    "OpenBookmarksInNewTab")},
    {"multi-column-bookmarks",
     "Multi-column Bookmarks",
     "Displays long bookmark folder menus in columns within the screen work area. "
     "Menus scroll vertically when all available columns are full. Restart required.",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kMultiColumnBookmarks)},
    {"open-omnibox-url-in-new-tab",
     "Open Omnibox URLs in New Tab",
     "Opens navigations from the address bar in a foreground or background new tab instead of the current tab.",
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kOpenOmniboxUrlInNewTab,
                                    kOpenOmniboxUrlInNewTabVariations,
                                    "OpenOmniboxUrlInNewTab")},
    {"close-confirmation",
     "Close Confirmation",
     "Show a warning prompt when closing browser window(s).",
     kOsDesktop, MULTI_VALUE_TYPE(kCloseConfirmation)},
    {"enable-incognito-themes",
     "Enable Incognito Themes",
     "Allows Incognito windows and web content to follow the browser theme color mode.",
     kOsDesktop, SINGLE_VALUE_TYPE("enable-incognito-themes")},
    {"close-window-with-last-tab",
     "Close window with last tab",
     "Determines whether a window should close once the last tab is closed.",
     kOsDesktop, MULTI_VALUE_TYPE(kCloseWindowWithLastTab)},

#if !BUILDFLAG(IS_ANDROID)
    {"media-router",
     "Enable/Disable Media Router",
     "Media router is a component responsible for pairing Thorium to devices and endpoints, "
     "for streaming and rendering media sources on those devices. This is used, for example, for Cast.",
     kOsDesktop, FEATURE_VALUE_TYPE(media_router::kMediaRouter)},
#endif // BUILDFLAG(IS_ANDROID)

    {"show-fps-counter",
     "Show FPS Counter",
     "Draws a heads-up-display showing Frames Per Second as well as GPU memory usage.",
     kOsAll, SINGLE_VALUE_TYPE(switches::kShowFPSCounter)},
    {"disable-webgl2",
     "Disable WebGL 2",
     "Disable WebGL 2. Useful for certain GPU/OS combinations.",
     kOsAll, SINGLE_VALUE_TYPE(switches::kDisableWebGL2)},
    {"allow-file-access-from-files",
     "Allow File URI Access from Files",
     "By default, file:// URIs cannot read other file:// URIs. This is an override for web developers who need this behavior for testing.",
     kOsAll, SINGLE_VALUE_TYPE(switches::kAllowFileAccessFromFiles)},
    {"disable-web-security",
     "Disable Web Security",
     "Don't enforce the same-origin policy; meant for website testing only. See `https://web.dev/same-origin-policy/`",
     kOsAll, SINGLE_VALUE_TYPE(switches::kDisableWebSecurity)},
    {"disable-encryption",
     "Disable Encryption",
     "Disable encryption of cookies, passwords, and settings which normally uses a generated machine-specific encryption key. "
     "This is used to enable portable user data directories. Enabled for Thorium Portable.",
     kOsDesktop, SINGLE_VALUE_TYPE("disable-encryption")},
    {"disable-machine-id",
     "Disable Machine ID",
     "Disables use of a generated machine-specific ID to lock the user data directory to that machine. This is used to enable portable user data directories. Enabled for Thorium Portable.",
     kOsDesktop, SINGLE_VALUE_TYPE("disable-machine-id")},
    {"revert-from-portable",
     "Prevent Data Loss When Changing User Profile Portable State",
     "When moving a Thorium user profile from one drive to another (or one system to another), enable this flag before moving the profile directory. It should also be used "
     "when migrating a portable profile back to a normal, non-portable profile (i.e. when disabling the `chrome://flags#disable-encryption` and/or the `chrome://flags#disable-machine-id` flags "
     "after being previously enabled). This mostly ensures that extensions, extension data, and some other data are not lost during the migration. When you are done migrating, the flag should be "
     "reset back to the default (Disabled).",
     kOsDesktop, SINGLE_VALUE_TYPE("revert-from-portable")},

#if BUILDFLAG(IS_LINUX)
    {"password-store",
     "Password Store Backend",
     "Choose the password store backend, instead of using the automatically detected one. "
     "Sometimes the default detected backend is incorrect, or you would want `Basic`, "
     "instead of the platform provided password stores on Linux. (i.e. for portable usage.)",
     kOsLinux, MULTI_VALUE_TYPE(kPasswordStoreChoices)},
#endif // BUILDFLAG(IS_LINUX)

#if BUILDFLAG(IS_WIN)
    {"enable-exclusive-audio",
     "Enable Exclusive Audio Streams",
     "Use exclusive mode audio streaming for Windows Vista and higher. Leads to lower latencies for audio streams which use the AudioParameters::AUDIO_PCM_LOW_LATENCY audio path. "
     "See https://docs.microsoft.com/en-us/windows/win32/coreaudio/exclusive-mode-streams for details.",
     kOsWin, SINGLE_VALUE_TYPE(switches::kEnableExclusiveAudio)},
#endif // BUILDFLAG(IS_WIN)

#endif  // CHROME_BROWSER_THORIUM_FLAG_ENTRIES_H_

// kDisableWindows10CustomTitlebar
