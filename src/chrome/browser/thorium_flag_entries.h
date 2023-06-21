// Copyright (c) 2023 Alex313031. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_THORIUM_FLAG_ENTRIES_H_
#define CHROME_BROWSER_THORIUM_FLAG_ENTRIES_H_

    {"force-dark-mode",
     "Enable Dark Mode",
     "Enables dark mode for all Thorium instances.",
     kOsDesktop, SINGLE_VALUE_TYPE(switches::kForceDarkMode)},

#if BUILDFLAG(IS_LINUX)
    {"auto-dark-mode",
     "GTK Auto Dark Mode",
     "Enables Thorium to automatically change to Dark Mode according to the system GTK Theme.",
     kOsLinux, SINGLE_VALUE_TYPE("auto-dark-mode")},
#endif // BUILDFLAG(IS_LINUX)

    {"force-high-contrast",
     "Enable High Contrast Mode",
     "Enables high contrast mode for all Thorium instances.",
     kOsAll, SINGLE_VALUE_TYPE("force-high-contrast")},
    {"tab-outlines-in-low-contrast-themes",
     "Tab Outlines in Low Contrast Themes",
     "Expands the range of situations in which tab outline strokes are displayed, improving accessiblity in dark and incognito mode.",
     kOsAll, SINGLE_VALUE_TYPE("tab-outlines-in-low-contrast-themes")},
    {"prominent-dark-mode-active-tab-title",
     "Prominent Dark Mode Active Tab Titles",
     "Makes the active tab title bolder in dark mode so that it is easier to identify.",
     kOsAll, SINGLE_VALUE_TYPE("prominent-dark-mode-active-tab-title")},
    {"custom-ntp",
     "Custom New Tab Page",
     "Allows setting a custom URL for the New Tab Page (NTP). Value can be internal (e.g. `about:blank` or `chrome://new-tab-page`), external (e.g. `example.com`), or local (e.g. `file:///tmp/startpage.html`). This applies for incognito windows as well when not set to a `chrome://` internal page.",
     kOsDesktop, ORIGIN_LIST_VALUE_TYPE("custom-ntp", "")},
    {"hide-sidepanel-button",
     "Hide Side Panel Button",
     "Hides the Thorium Side Panel Button.",
     kOsDesktop, SINGLE_VALUE_TYPE("hide-sidepanel-button")},
    {"scroll-tabs",
     "Scroll Switches Active Tab",
     "Switch to the left/right tab if a scroll wheel event happens over the tabstrip, or the empty space beside the tabstrip.",
     kOsDesktop, MULTI_VALUE_TYPE(kScrollEventChangesTab)},
    {"autoplay-policy",
     "Disable/Enable AutoPlay",
     "Allows setting the AutoPlay policy. Use `No User Gesture Required` to enable AutoPlay, and use `Document User Activation Required` to disable AutoPlay and force all sites to require a click to initiate media playback. `User Gesture Required` is the default, and blocks most AutoPlay annoyances, while allowing some (i.e. WebAudio) to continue.",
     kOsDesktop, MULTI_VALUE_TYPE(kAutoplayPolicyChoices)},
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
     kOsAll, MULTI_VALUE_TYPE(kForceGpuMemAvailableMbChoices)},

#if BUILDFLAG(IS_LINUX)
    {"enable-native-gpu-memory-buffers",
     "Enable Native GPU Memory Buffers",
     "Enables native CPU-mappable GPU memory buffer support on Linux.",
     kOsLinux, SINGLE_VALUE_TYPE(switches::kEnableNativeGpuMemoryBuffers)},
#endif // BUILDFLAG(IS_LINUX)

    {"gpu-no-context-lost",
     "No GPU Context Lost",
     "Inform Thorium's GPU process that a GPU context will not be lost in power saving mode, screen saving mode, etc. "
     "Note that this flag does not ensure that a GPU context will never be lost in any situation, like say, a GPU reset. "
     "Useful for fixing blank or pink screens/videos upon system resume, etc.",
     kOsAll, SINGLE_VALUE_TYPE(switches::kGpuNoContextLost)},
    {"enable-ui-devtools",
     "Enable Native UI Inspection in DevTools",
     "Enables inspection of native UI elements in devtools. Inspect at `chrome://inspect/#native-ui`",
     kOsAll, SINGLE_VALUE_TYPE(ui_devtools::switches::kEnableUiDevTools)},
    {"double-click-close-tab",
     "Double Click to Close Tab",
     "Enables double clicking a tab to close it.",
     kOsDesktop, SINGLE_VALUE_TYPE("double-click-close-tab")},
    {"show-fps-counter",
     "Show FPS Counter",
     "Draws a heads-up-display showing Frames Per Second as well as GPU memory usage.",
     kOsAll, SINGLE_VALUE_TYPE(cc::switches::kShowFPSCounter)},
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
     "By default, file:// URIs cannot read other file:// URIs. This is an override for web developers who need the old behavior for testing.",
     kOsAll, SINGLE_VALUE_TYPE(switches::kAllowFileAccessFromFiles)},
    {"disable-web-security",
     "Disable Web Security",
     "Don't enforce the same-origin policy; meant for website testing only. See `https://web.dev/same-origin-policy/`",
     kOsAll, SINGLE_VALUE_TYPE(switches::kDisableWebSecurity)},

#if !BUILDFLAG(IS_ANDROID)
    {"media-router",
     "Enable/Disable Media Router",
     "Media router is a component responsible for pairing Thorium to devices and endpoints, for streaming and rendering media sources on those devices. This is used, for example, for Cast.",
     kOsDesktop, FEATURE_VALUE_TYPE(media_router::kMediaRouter)},
#endif // BUILDFLAG(IS_ANDROID)

    {"disable-encryption",
     "Disable Encryption",
     "Disable encryption of cookies, passwords, and settings which normally uses a generated machine-specific encryption key. This is used to enable portable user data directories. Enabled for Thorium Portable.",
     kOsDesktop, SINGLE_VALUE_TYPE("disable-encryption")},
    {"disable-machine-id",
     "Disable Machine ID",
     "Disables use of a generated machine-specific ID to lock the user data directory to that machine. This is used to enable portable user data directories. Enabled for Thorium Portable.",
     kOsDesktop, SINGLE_VALUE_TYPE("disable-machine-id")},
    {"close-confirmation",
     "Close Confirmation",
     "Show a warning prompt when closing browser window(s).",
     kOsDesktop, MULTI_VALUE_TYPE(kCloseConfirmation)},

#if BUILDFLAG(IS_LINUX)
    {"password-store",
     "Password Store Backend",
     "Choose the password store backend, instead of using the automatically detected one. "
     "Sometimes the default detected backend is incorrect, or you would want `Basic`, "
     "instead of the platform provided password stores on Linux. (i.e. for portable usage.)",
     kOsLinux, MULTI_VALUE_TYPE(kPasswordStoreChoices)},
#endif // BUILDFLAG(IS_LINUX)

#if BUILDFLAG(IS_WIN)
    {"disable-windows10-custom-titlebar",
     "Disable Custom Windows Titlebar",
     "Disables custom-drawing the window titlebar on Windows 10/11.",
     kOsWin, SINGLE_VALUE_TYPE(switches::kDisableWindows10CustomTitlebar)},
    {"enable-exclusive-audio",
     "Enable Exclusive Audio Streams",
     "Use exclusive mode audio streaming for Windows Vista and higher. Leads to lower latencies for audio streams which use the AudioParameters::AUDIO_PCM_LOW_LATENCY audio path. "
     "See https://docs.microsoft.com/en-us/windows/win32/coreaudio/exclusive-mode-streams for details.",
     kOsWin, SINGLE_VALUE_TYPE(switches::kEnableExclusiveAudio)},
#endif // BUILDFLAG(IS_WIN)

#endif  // CHROME_BROWSER_THORIUM_FLAG_ENTRIES_H_
