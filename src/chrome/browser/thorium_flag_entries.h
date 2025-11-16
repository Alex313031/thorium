// Copyright (c) 2024 Alex313031.
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
#endif // BUILDFLAG(IS_LINUX)

    {"thorium-2024",
     "Enable Experimental Thorium 2024 (Th24) UI",
     "Enable a new \"hybrid\" UI, which restores many parts of the pre-Chrome Refresh 2023 UI. Good for people "
     "who find the new UI ugly or harder to use.",
     kOsDesktop, FEATURE_VALUE_TYPE(features::kThorium2024)},
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
     "Disable Thorium Top Bar Icons",
     "Disables the custom colored top bar icons in Thorium, and restores the default grey Chromium icon colors.",
     kOsDesktop, SINGLE_VALUE_TYPE("disable-thorium-icons")},
    {"left-aligned-tab-search-button",
     "Left Aligned Tab Search Button",
     "The default placement of the tab search button was changed to the left of the tabstrip in M125, but Thorium has reverted this back "
     "to the original righthand placement. This flag is for people who want the new CR23 left aligned behavior.",
     kOsDesktop, SINGLE_VALUE_TYPE("left-aligned-tab-search-button")},
    {"remove-tabsearch-button",
     "Remove Tab Search Button",
     "Removes the tabsearch button from the tabstrip.",
     kOsDesktop, SINGLE_VALUE_TYPE("remove-tabsearch-button")},

#if BUILDFLAG(IS_WIN)
    {"tab-search-caption-button",
     "Disable Windows Tab Search Caption Button",
     "Disables the tab search caption button (i.e. the version of the button that's adjacent to, and mimics, the minimize/maximize/close buttons), and "
     "restores the standard rounded tab search button on the right of the tabstrip.",
     kOsDesktop, SINGLE_VALUE_TYPE("disable-caption-button")},
#endif // BUILDFLAG(IS_WIN)

    {"classic-omnibox",
     "Classic Omnibox UI",
     "Changes the omnibox shape to be more square.",
     kOsDesktop, SINGLE_VALUE_TYPE("classic-omnibox")},
    {"rectangular-tabs",
     "Thorium Rectangular Tabs UI",
     "Changes the look of browser tabs to appear with a rectangular shape, similar to Vivaldi or Cent Browser.",
     kOsDesktop, SINGLE_VALUE_TYPE("rectangular-tabs")},

//#if BUILDFLAG(IS_WIN)
    //{"transparent-tabs",
     //"Thorium Semi-Transparent Tabs UI",
     //"Reduces the opacity of tabs.",
     //kOsWin, SINGLE_VALUE_TYPE("transparent-tabs")},
//#endif // BUILDFLAG(IS_WIN)

    {"custom-tab-width",
     "Custom Tab Width",
     "Allows setting the default tab width, in DIP. Normally 1 DIP = 1 Pixel, and the standard width for tabs is 240.",
     kOsAll, MULTI_VALUE_TYPE(kCustomTabWidthChoices)},
    {"disable-thorium-dns-config",
     "Disable Thorium Custom DNS Config",
     "Disables the custom DNS configuration used by default in Thorium. Useful when this config breaks something, "
     "due to external apps or a non-standard system DNS config setting.",
     kOsDesktop, SINGLE_VALUE_TYPE("disable-thorium-dns-config")},
    //{"side-panel-journeys",
     //"Side Panel Journeys",
     //"Enables Journeys within the Side Panel.",
     //kOsDesktop, FEATURE_VALUE_TYPE(history_clusters::kSidePanelJourneys)},
    // {"customize-chrome-side-panel-no-cr23",
     // "Customize Chrome Side Panel (Non-Cr23 Variant)",
     // "Enables the \"Customize Chrome\" item in the Side Panel, without the Cr23 UI, and without having to also enable Chrome Refresh 2023.",
     // kOsDesktop, FEATURE_VALUE_TYPE(features::kCustomizeChromeSidePanelNoChromeRefresh2023)},

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
     kOsDesktop, FEATURE_VALUE_TYPE(blink::features::kMiddleClickAutoscroll)},
#endif // BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)

    {"autoplay-policy",
     "Configure AutoPlay Policy",
     "Allows setting the AutoPlay policy. `No User Gesture Required` enables AutoPlay. `Document User Activation Required` disables AutoPlay, "
     "and forces all sites to require a click to initiate media playback; this is the default if unset. `User Gesture Required` blocks "
     "most AutoPlay annoyances, while still allowing some (i.e. WebAudio) to continue.",
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
#endif // BUILDFLAG(IS_ANDROID)

    {"show-avatar-button",
     "Show/Hide the Avatar Button",
     "Show the Avatar/People/Profile button in the browser toolbar: Always, Incognito|Guest only, or Never.",
     kOsDesktop, MULTI_VALUE_TYPE(kShowAvatarButtonChoices)},
    {"keep-all-history",
     "Keep All History",
     "Retain All local browsing history. By default history older than 4 months is expired and purged. Thorium flag",
     kOsAll, SINGLE_VALUE_TYPE("keep-all-history")},
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
    {"enable-ui-devtools",
     "Enable Native UI Inspection in DevTools",
     "Enables inspection of native UI elements in devtools. Inspect at `chrome://inspect/#native-ui`",
     kOsAll, SINGLE_VALUE_TYPE(ui_devtools::switches::kEnableUiDevTools)},
    {"tab-hover-cards",
     "Tab Hover Cards",
     "Allows removing the tab hover cards or using a tooltip as a replacement.",
     kOsDesktop, MULTI_VALUE_TYPE(kTabHoverCardChoices)},
    {"double-click-close-tab",
     "Double Click to Close Tab",
     "Enables double clicking a tab to close it.",
     kOsDesktop, SINGLE_VALUE_TYPE("double-click-close-tab")},
    {"close-confirmation",
     "Close Confirmation",
     "Show a warning prompt when closing browser window(s).",
     kOsDesktop, MULTI_VALUE_TYPE(kCloseConfirmation)},
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
    {"enable-local-file-accesses",
     "Enable Local File Accesses",
     "Enable local file accesses. Useful for web development.",
     kOsAll, SINGLE_VALUE_TYPE("enable-local-file-accesses")},
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
