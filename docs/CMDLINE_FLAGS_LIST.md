## There are many command line flags which can be used with Chromium. Some change behavior or features, others are for debugging or experimenting. This page lists the available switches including their conditions and descriptions.

Format: Condition ## Explanation ##
-----------------------------------

--/prefetch:1[1] ##	/prefetch:# arguments to use when launching various process types. It has been observed that when file reads are consistent for 3 process launches with the same /prefetch:# argument, the Windows prefetcher starts issuing reads in batch at process launch. Because reads depend on the process type, the prefetcher wouldn't be able to observe consistent reads if no /prefetch:# arguments were used. Note that the browser process has no /prefetch:# argument; as such all other processes must have one in order to avoid polluting its profile. Note: # must always be in [1, 8]; otherwise it is ignored by the Windows prefetcher. ##

--/prefetch:2[1] ##	No description ##

--/prefetch:3[1] ##	No description ##

--/prefetch:4[1] ##	No description ##

--/prefetch:5[1] ##	/prefetch:# arguments for the browser process launched in background mode and for the watcher process. Use profiles 5, 6 and 7 as documented on kPrefetchArgument\* in content\_switches.cc. ##

--/prefetch:6[1] ##	No description ##

--/prefetch:8[1] ##	Prefetch arguments are used by the Windows prefetcher to disambiguate different execution modes (i.e. process types) of the same executable image so that different types of processes don't trample each others' prefetch behavior. Legal values are integers in the range [1, 8]. We reserve 8 to mean "whatever", and this will ultimately lead to processes with /prefetch:8 having inconsistent behavior thus disabling prefetch in practice. TODO(rockot): Make it possible for embedders to override this argument on a per-service basis. ##

--10000 ##	No description ##

--100000 ##	No description ##

--1000000 ##	No description ##

--3d-display-mode[1] ##	No description ##

--50000 ##	No description ##

--500000 ##	No description ##

--5000000 ##	No description ##

--? ##	No description ##

--accept-resource-provider ##	Flag indicating that a resource provider must be set up to provide cast receiver with resources. Apps cannot start until provided resources. This flag implies --alsa-check-close-timeout=0. ##

--adaboost ##	No description ##

--add-gpu-appcontainer-caps[1] ##	Add additional capabilities to the AppContainer sandbox on the GPU process. ##

--add-xr-appcontainer-caps[1] ##	Add additional capabilities to the AppContainer sandbox used for XR compositing. ##

--additional-trust-token-key-commitments ##	Manually sets additional Trust Tokens key commitments in the network service to the given value, which should be a JSON dictionary satisfying the requirements of TrustTokenKeyCommitmentParser::ParseMultipleIssuers. These keys are available in addition to keys provided by the most recent call to TrustTokenKeyCommitments::Set. For issuers with keys provided through both the command line and TrustTokenKeyCommitments::Set, the keys provided through the command line take precedence. This is because someone testing manually might want to pass additional keys via the command line to a real Chrome release with the component updater enabled, and it would be surprising if the manually-passed keys were overwritten some time after startup when the component updater runs. ##

--agc-startup-min-volume ##	Override the default minimum starting volume of the Automatic Gain Control algorithm in WebRTC used with audio tracks from getUserMedia. The valid range is 12-255. Values outside that range will be clamped to the lowest or highest valid value inside WebRTC. TODO(tommi): Remove this switch when crbug.com/555577 is fixed. ##

--aggressive ##	No description ##

--aggressive-cache-discard ##	No description ##

--aggressive-tab-discard ##	No description ##

--all ##	No description ##

--all-renderers ##	No description ##

--allarticles ##	No description ##

--allow-cross-origin-auth-prompt ##	Allows third-party content included on a page to prompt for a HTTP basic auth username/password pair. ##

--allow-external-pages ##	Allow access to external pages during web tests. ##

--allow-failed-policy-fetch-for-test ##	If this flag is passed, failed policy fetches will not cause profile initialization to fail. This is useful for tests because it means that tests don't have to mock out the policy infrastructure. ##

--allow-file-access-from-files ##	By default, <file://> URIs cannot read other <file://> URIs. This is an override for developers who need the old behavior for testing. ##

--allow-http-background-page ##	Allows non-https URL for background\_page for hosted apps. ##

--allow-http-screen-capture ##	Allow non-secure origins to use the screen capture API and the desktopCapture extension API. ##

--allow-insecure-localhost ##	Enables TLS/SSL errors on localhost to be ignored (no interstitial, no blocking of requests). ##

--allow-legacy-extension-manifests ##	Allows the browser to load extensions that lack a modern manifest when that would otherwise be forbidden. ##

--allow-loopback-in-peer-connection ##	Allows loopback interface to be added in network list for peer connection. ##

--allow-nacl-crxfs-api[2] ##	Specifies comma-separated list of extension ids or hosts to grant access to CRX file system APIs. ##

--allow-nacl-file-handle-api[2] ##	Specifies comma-separated list of extension ids or hosts to grant access to file handle APIs. ##

--allow-nacl-socket-api[2] ##	Specifies comma-separated list of extension ids or hosts to grant access to TCP/UDP socket APIs. ##

--allow-no-sandbox-job ##	Enables the sandboxed processes to run without a job object assigned to them. This flag is required to allow Chrome to run in RemoteApps or Citrix. This flag can reduce the security of the sandboxed processes and allow them to do certain API calls like shut down Windows or access the clipboard. Also we lose the chance to kill some processes until the outer job that owns them finishes. ##

--allow-outdated-plugins ##	Don't block outdated plugins. ##

--allow-popups-during-page-unload ##	Allow a page to show popups during its unloading. TODO(https://crbug.com/937569): Remove this in Chrome 88. ##

--allow-pre-commit-input ##	Allows processing of input before a frame has been committed. TODO(schenney): crbug.com/987626. Used by headless. Look for a way not involving a command line switch. ##

--allow-profiles-outside-user-dir ##	Allows profiles to be created outside of the user data dir. TODO(https://crbug.com/1060366): Various places in Chrome assume that all profiles are within the user data dir. Some tests need to violate that assumption. The switch should be removed after this workaround is no longer needed. ##

--allow-ra-in-dev-mode ##	Allows remote attestation (RA) in dev mode for testing purpose. Usually RA is disabled in dev mode because it will always fail. However, there are cases in testing where we do want to go through the permission flow even in dev mode. This can be enabled by this flag. ##

--allow-running-insecure-content ##	No description ##

--allow-sandbox-debugging ##	Allows debugging of sandboxed processes (see zygote\_main\_linux.cc). ##

--allow-silent-push ##	Allows Web Push notifications that do not show a notification. ##

--allow-sync-xhr-in-page-dimissal ##	Allow a page to send synchronous XHR during its unloading. TODO(https://crbug.com/1003101): Remove this in Chrome 88. ##

--allow-third-party-modules[1] ##	Allows third party modules to inject by disabling the BINARY\_SIGNATURE mitigation policy on Win10+. Also has other effects in ELF. ##

--allow-unsecure-dlls[3] ##	Don't allow EnableSecureDllLoading to run when this is set. This is only to be used in tests. ##

--alsa-amp-device-name ##	Name of the device the amp mixer should be opened on. If this flag is not specified it will default to the same device as kAlsaVolumeDeviceName. ##

--alsa-amp-element-name ##	Name of the simple mixer control element that the ALSA-based media library should use to toggle powersave mode on the system. ##

--alsa-check-close-timeout ##	Time in ms to wait before closing the PCM handle when no more mixer inputs remain. Assumed to be 0 if --accept-resource-provider is present. ##

--alsa-enable-upsampling ##	Flag that enables resampling audio with sample rate below 32kHz up to 48kHz. Should be set to true for internal audio products. ##

--alsa-fixed-output-sample-rate ##	Optional flag to set a fixed sample rate for the alsa device. Deprecated: Use --audio-output-sample-rate instead. ##

--alsa-input-device[4] ##	The Alsa device to use when opening an audio input stream. ##

--alsa-mute-device-name ##	Name of the device the mute mixer should be opened on. If this flag is not specified it will default to the same device as kAlsaVolumeDeviceName. ##

--alsa-mute-element-name ##	Name of the simple mixer control element that the ALSA-based media library should use to mute the system. ##

--alsa-output-avail-min ##	Minimum number of available frames for scheduling a transfer. ##

--alsa-output-buffer-size ##	Size of the ALSA output buffer in frames. This directly sets the latency of the output device. Latency can be calculated by multiplying the sample rate by the output buffer size. ##

--alsa-output-device[4] ##	The Alsa device to use when opening an audio stream. ##

--alsa-output-period-size ##	Size of the ALSA output period in frames. The period of an ALSA output device determines how many frames elapse between hardware interrupts. ##

--alsa-output-start-threshold ##	How many frames need to be in the output buffer before output starts. ##

--alsa-volume-device-name ##	Name of the device the volume control mixer should be opened on. Will use the same device as kAlsaOutputDevice and fall back to "default" if kAlsaOutputDevice is not supplied. ##

--alsa-volume-element-name ##	Name of the simple mixer control element that the ALSA-based media library should use to control the volume. ##

--also-emit-success-logs ##	Also emit full event trace logs for successful tests. ##

--alt1 ##	The Chrome-Proxy "exp" directive value used by data reduction proxy to receive an alternative back end implementation. ##

--alt10 ##	No description ##

--alt2 ##	No description ##

--alt3 ##	No description ##

--alt4 ##	No description ##

--alt5 ##	No description ##

--alt6 ##	No description ##

--alt7 ##	No description ##

--alt8 ##	No description ##

--alt9 ##	No description ##

--always-use-complex-text ##	Always use the complex text path for web tests. ##

--alwaystrue ##	No description ##

--android-fonts-path ##	Uses the android SkFontManager on linux. The specified directory should include the configuration xml file with the name "fonts.xml". This is used in blimp to emulate android fonts on linux. ##

--angle ##	No description ##

--animation-duration-scale ##	Scale factor to apply to every animation duration. Must be >= 0.0. This will only apply to LinearAnimation and its subclasses. ##

--app ##	Specifies that the associated value should be launched in "application" mode. ##

--app-auto-launched ##	Specifies whether an app launched in kiosk mode was auto launched with zero delay. Used in order to properly restore auto-launched state during session restore flow. ##

--app-cache-force-enabled ##	Allows app cache to be forced on, even when gated by an origin trial. TODO(enne): remove this once app cache has been removed. ##

--app-id ##	Specifies that the extension-app with the specified id should be launched according to its configuration. ##

--app-launch-url-for-shortcuts-menu-item ##	Overrides the launch url of an app with the specified url. This is used along with kAppId to launch a given app with the url corresponding to an item in the app's shortcuts menu. ##

--app-mode-auth-code ##	Value of GAIA auth code for --force-app-mode. ##

--app-mode-oauth-token ##	Value of OAuth2 refresh token for --force-app-mode. ##

--app-mode-oem-manifest ##	Path for app's OEM manifest file. ##

--app-shell-allow-roaming[5] ##	Allow roaming in the cellular network. ##

--app-shell-host-window-size[5] ##	Size for the host window to create (i.e. "800x600"). ##

--app-shell-preferred-network[5] ##	SSID of the preferred WiFi network. ##

--apple ##	No description ##

--apps-gallery-download-url ##	The URL that the webstore APIs download extensions from. Note: the URL must contain one '%s' for the extension ID. ##

--apps-gallery-update-url ##	The update url used by gallery/webstore extensions. ##

--apps-gallery-url ##	The URL to use for the gallery link in the app launcher. ##

--apps-keep-chrome-alive-in-tests[6] ##	Prevents Chrome from quitting when Chrome Apps are open. ##

--arc-availability ##	Signals ARC support status on this device. This can take one of the following three values. - none: ARC is not installed on this device. (default) - installed: ARC is installed on this device, but not officially supported. Users can enable ARC only when Finch experiment is turned on. - officially-supported: ARC is installed and supported on this device. So users can enable ARC via settings etc. ##

--arc-available ##	DEPRECATED: Please use --arc-availability=installed. Signals the availability of the ARC instance on this device. ##

--arc-build-properties ##	A JSON dictionary whose content is the same as cros config's /arc/build-properties. ##

--arc-data-cleanup-on-start ##	Flag that forces ARC data be cleaned on each start. ##

--arc-disable-app-sync ##	Flag that disables ARC app sync flow that installs some apps silently. Used in autotests to resolve racy conditions. ##

--arc-disable-gms-core-cache ##	Used in autotest to disable GMS-core caches which is on by default. ##

--arc-disable-locale-sync ##	Flag that disables ARC locale sync with Android container. Used in autotest to prevent conditions when certain apps, including Play Store may get restarted. Restarting Play Store may cause random test failures. Enabling this flag would also forces ARC container to use 'en-US' as a locale and 'en-US,en' as preferred languages. ##

--arc-disable-play-auto-install ##	Flag that disables ARC Play Auto Install flow that installs set of predefined apps silently. Used in autotests to resolve racy conditions. ##

--arc-disable-system-default-apps ##	Used for development of Android app that are included into ARC++ as system default apps in order to be able to install them via adb. ##

--arc-enable-native-bridge-64bit-support-experiment ##	Flag to enables an experiment to allow users to turn on 64-bit support in native bridge on systems that have such support available but not yet enabled by default. ##

--arc-force-cache-app-icons ##	Flag that forces ARC to cache icons for apps. ##

--arc-force-show-optin-ui ##	Flag that forces the OptIn ui to be shown. Used in tests. ##

--arc-packages-cache-mode ##	Used in autotest to specifies how to handle packages cache. Can be copy - copy resulting packages.xml to the temporary directory. skip-copy - skip initial packages cache setup and copy resulting packages.xml to the temporary directory. ##

--arc-play-store-auto-update ##	Used in autotest to forces Play Store auto-update state. Can be on - auto-update is forced on. off - auto-update is forced off. ##

--arc-scale ##	Set the scale for ARC apps. This is in DPI. e.g. 280 DPI is ~ 1.75 device scale factor. See <https://source.android.com/compatibility/android-cdd#3_7_runtime_compatibility> for list of supported DPI values. ##

--arc-start-mode ##	Defines how to start ARC. This can take one of the following values: - always-start automatically start with Play Store UI support. - always-start-with-no-play-store automatically start without Play Store UI. If it is not set, then ARC is started in default mode. ##

--arc-tos-host-for-tests ##	Sets ARC Terms Of Service hostname url for testing. ##

--arc-transition-migration-required ##	If this flag is present then the device had ARC M available and gets ARC N when updating. TODO(pmarko): Remove this when we assess that it's not necessary anymore: crbug.com/761348. ##

--as-browser ##	Flag to launch tests in the browser process. ##

--ash-color-mode ##	Indicates the current color mode of ash. ##

--ash-constrain-pointer-to-root ##	Force the pointer (cursor) position to be kept inside root windows. ##

--ash-contextual-nudges-interval ##	Overrides the minimum time that must pass between showing user contextual nudges. Unit of time is in seconds. ##

--ash-contextual-nudges-reset-shown-count ##	Reset contextual nudge shown count on login. ##

--ash-debug-shortcuts ##	Enable keyboard shortcuts useful for debugging. ##

--ash-dev-shortcuts ##	Enable keyboard shortcuts used by developers only. ##

--ash-disable-touch-exploration-mode ##	Disable the Touch Exploration Mode. Touch Exploration Mode will no longer be turned on automatically when spoken feedback is enabled when this flag is set. ##

--ash-enable-cursor-motion-blur ##	Enable cursor motion blur. ##

--ash-enable-magnifier-key-scroller ##	Enables key bindings to scroll magnified screen. ##

--ash-enable-palette-on-all-displays ##	Enables the palette on every display, instead of only the internal one. ##

--ash-enable-software-mirroring ##	Enables software based mirroring. ##

--ash-enable-unified-desktop[5] ##	Enables unified desktop mode. ##

--ash-enable-v1-app-back-button ##	Enables Backbutton on frame for v1 apps. TODO(oshima): Remove this once the feature is launched. crbug.com/749713. ##

--ash-hide-notifications-for-factory ##	Hides notifications that are irrelevant to Chrome OS device factory testing, such as battery level updates. ##

--ash-host-window-bounds ##	Sets a window size, optional position, and optional scale factor. "1024x768" creates a window of size 1024x768. "100+200-1024x768" positions the window at 100,200. "1024x768\*2" sets the scale factor to 2 for a high DPI display. "800,0+800-800x800" for two displays at 800x800 resolution. "800,0+800-800x800,0+1600-800x800" for three displays at 800x800 resolution. ##

--ash-power-button-position ##	Power button position includes the power button's physical display side and the percentage for power button center position to the display's width/height in landscape\_primary screen orientation. The value is a JSON object containing a "position" property with the value "left", "right", "top", or "bottom". For "left" and "right", a "y" property specifies the button's center position as a fraction of the display's height (in [0.0, 1.0]) relative to the top of the display. For "top" and "bottom", an "x" property gives the position as a fraction of the display's width relative to the left side of the display. ##

--ash-side-volume-button-position ##	The physical position info of the side volume button while in landscape primary screen orientation. The value is a JSON object containing a "region" property with the value "keyboard", "screen" and a "side" property with the value "left", "right", "top", "bottom". ##

--ash-touch-hud ##	Enables the heads-up display for tracking touch points. ##

--attestation-server ##	Used in CryptohomeClient to determine which Google Privacy CA to use for attestation. ##

--audio ##	No description ##

--audio-buffer-size ##	Allow users to specify a custom buffer size for debugging purpose. ##

--audio-output-channels ##	Number of audio output channels. This will be used to send audio buffer with specific number of channels to ALSA and generate loopback audio. Default value is 2. ##

--audio-output-sample-rate ##	Specify fixed sample rate for audio output stream. If this flag is not specified the StreamMixer will choose sample rate based on the sample rate of the media stream. ##

--audio-service-quit-timeout-ms ##	Set a timeout (in milliseconds) for the audio service to quit if there are no client connections to it. If the value is negative the service never quits. ##

--aue-reached-for-update-required-test ##	If this switch is passed, the device policy DeviceMinimumVersion assumes that the device has reached Auto Update Expiration. This is useful for testing the policy behaviour on the DUT. ##

--aura-legacy-power-button ##	(Most) Chrome OS hardware reports ACPI power button releases correctly. Standard hardware reports releases immediately after presses. If set, we lock the screen or shutdown the system immediately in response to a press instead of displaying an interactive animation. ##

--auth-server-whitelist ##	Allowlist for Negotiate Auth servers. ##

--auth-spnego-account-type[7] ##	Android authentication account type for SPNEGO authentication ##

--auto ##	No description ##

--auto-open-devtools-for-tabs ##	This flag makes Chrome auto-open DevTools window for each tab. It is intended to be used by developers and automation to not require user interaction for opening DevTools. ##

--auto-select-desktop-capture-source ##	This flag makes Chrome auto-select the provided choice when an extension asks permission to start desktop capture. Should only be used for tests. For instance, --auto-select-desktop-capture-source="Entire screen" will automatically select sharing the entire screen in English locales. The switch value only needs to be substring of the capture source name, i.e. "display" would match "Built-in display" and "External display", whichever comes first. ##

--autofill-api-key ##	Sets the API key that will be used when calling Autofill API instead of using Chrome's baked key by default. You can use this to test new versions of the API that are not linked to the Chrome baked key yet. ##

--autofill-assistant-auth ##	Disables authentication when set to false. This is only useful during development, as prod instances require authentication. ##

--autofill-assistant-key ##	Sets the API key to be used instead of Chrome's default key when sending requests to the backend. ##

--autofill-assistant-url ##	Overrides the default backend URL. ##

--autofill-ios-delay-between-fields ##	The delay between filling two fields. ##

--autofill-metadata-upload-encoding ##	The randomized encoding type to use when sending metadata uploads. The value of the parameter must be one of the valid integer values of the AutofillRandomizedValue\_EncodingType enum. ##

--autofill-server-url ##	Override the default autofill server URL with "scheme://host[:port]/prefix/". ##

--autofill-upload-throttling-period-in-days ##	The number of days after which to reset the registry of autofill events for which an upload has been sent. ##

--autoplay-policy ##	Command line flag name to set the autoplay policy. ##

--autopush ##	The command line alias and URL for the "autopush" environment. ##

--back-gesture-horizontal-threshold ##	The number of pixels from the start of a left swipe gesture to consider as a 'back' gesture. ##

--blink-settings ##	Set blink settings. Format is <name>[=<value],<name>[=<value>],... The names are declared in Settings.json5. For boolean type, use "true", "false", or omit '=<value>' part to set to true. For enum type, use the int value of the enum value. Applied after other command line flags and prefs. ##

--block-new-web-contents ##	If true, then all pop-ups and calls to window.open will fail. ##

--bootstrap ##	Values for the kExtensionContentVerification flag. See ContentVerifierDelegate::Mode for more explanation. ##

--bottom-gesture-start-height ##	The number of pixels up from the bottom of the screen to consider as a valid origin for a bottom swipe gesture. If set, overrides the value of both the above system-gesture-start-height flag and the default value in cast\_system\_gesture\_handler.cc. ##

--browser ##	No description ##

--browser-startup-dialog ##	Causes the browser process to display a dialog on launch. ##

--browser-subprocess-path ##	Path to the exe to run for the renderer and plugin subprocesses. ##

--browser-test ##	Tells whether the code is running browser tests (this changes the startup URL used by the content shell and also disables features that can make tests flaky [like monitoring of memory pressure]). ##

--bwsi ##	Indicates that the browser is in "browse without sign-in" (Guest session) mode. Should completely disable extensions, sync and bookmarks. ##

--bypass-app-banner-engagement-checks ##	This flag causes the user engagement checks for showing app banners to be bypassed. It is intended to be used by developers who wish to test that their sites otherwise meet the criteria needed to show app banners. ##

--cast-app-background-color ##	Background color used when Chromium hasn't rendered anything yet. ##

--cast-initial-screen-height ##	No description ##

--cast-initial-screen-width ##	Used to pass initial screen resolution to GPU process. This allows us to set screen size correctly (so no need to resize when first window is created). ##

--cc-layer-tree-test-long-timeout ##	Increases timeout for memory checkers. ##

--cc-layer-tree-test-no-timeout ##	Prevents the layer tree unit tests from timing out. ##

--cc-scroll-animation-duration-in-seconds ##	Controls the duration of the scroll animation curve. ##

--cdm ##	No description ##

--cellular-first ##	If this flag is set, it indicates that this device is a "Cellular First" device. Cellular First devices use cellular telephone data networks as their primary means of connecting to the internet. Setting this flag has two consequences: 1. Cellular data roaming will be enabled by default. 2. UpdateEngine will be instructed to allow auto-updating over cellular data connections. ##

--check-accessibility-permission[8] ##	No description ##

--check-damage-early ##	Checks damage early and aborts the frame if no damage, so that clients like Android WebView don't invalidate unnecessarily. ##

--check-for-update-interval ##	How often (in seconds) to check for updates. Should only be used for testing purposes. ##

--check-permission[8] ##	No description ##

--check-screen-recording-permission[8] ##	No description ##

--child-wallpaper-large ##	Default large wallpaper to use for kids accounts (as path to trusted, non-user-writable JPEG file). ##

--child-wallpaper-small ##	Default small wallpaper to use for kids accounts (as path to trusted, non-user-writable JPEG file). ##

--chrome-mojo-pipe-token ##	The Mojo pipe token for IPC communication between the Software Reporter and Chrome. Dropped in M80. ##

--ChromeOSMemoryPressureHandling ##	The memory pressure thresholds selection which is used to decide whether and when a memory pressure event needs to get fired. ##

--cipher-suite-blacklist ##	Comma-separated list of SSL cipher suites to disable. ##

--clamshell ##	Values for the kAshUiMode flag. ##

--class[9] ##	The same as the --class argument in X applications. Overrides the WM\_CLASS window property with the given value. ##

--cleaning-timeout ##	Set the timeout for the cleaning phase, in minutes. 0 disables the timeout entirely. WARNING: this switch is used by internal test systems. Be careful when making changes. ##

--cleanup-id ##	Identifier used to group all reports generated during the same run of the cleaner, including runs before and after a reboot. The id is generated by the first cleaner process for a run, and propagated to spawned or scheduled cleaner processes. A new id will be generated in the first process for a subsequent cleanup, so cleanups from the same user cannot be tracked over time. ##

--clear-key-cdm-path-for-testing ##	Specifies the path to the Clear Key CDM for testing, which is necessary to support External Clear Key key system when library CDM is enabled. Note that External Clear Key key system support is also controlled by feature kExternalClearKeyForTesting. ##

--clear-token-service ##	Clears the token service before using it. This allows simulating the expiration of credentials during testing. ##

--cloud-print-file ##	Tells chrome to display the cloud print dialog and upload the specified file for printing. ##

--cloud-print-file-type ##	Specifies the mime type to be used when uploading data from the file referenced by cloud-print-file. Defaults to "application/pdf" if unspecified. ##

--cloud-print-job-title ##	Used with kCloudPrintFile to specify a title for the resulting print job. ##

--cloud-print-print-ticket ##	Used with kCloudPrintFile to specify a JSON print ticket for the resulting print job. Defaults to null if unspecified. ##

--cloud-print-setup-proxy ##	Setup cloud print proxy for provided printers. This does not start service or register proxy for autostart. ##

--cloud-print-url ##	The URL of the cloud print service to use, overrides any value stored in preferences, and the default. Only used if the cloud print service has been enabled. Used for testing. ##

--cloud-print-xmpp-endpoint ##	The XMPP endpoint the cloud print service will use. Only used if the cloud print service has been enabled. Used for testing. ##

--compensate-for-unstable-pinch-zoom ##	Enable compensation for unstable pinch zoom. Some touch screens display significant amount of wobble when moving a finger in a straight line. This makes two finger scroll trigger an oscillating pinch zoom. See crbug.com/394380 for details. ##

--compile-shader-always-succeeds ##	Always return success when compiling a shader. Linking will still fail. ##

--component-updater ##	Comma-separated options to troubleshoot the component updater. Only valid for the browser process. ##

--connectivity-check-url ##	Url for network connectivity checking. Default is "<https://clients3.google.com/generate_204>". ##

--conservative ##	No description ##

--content-directories ##	No description ##

--content-shell-hide-toolbar ##	Hides toolbar from content\_shell's host window. ##

--content-shell-host-window-size ##	Size for the content\_shell's host window (i.e. "800x600"). ##

--controller ##	No description ##

--conversions-debug-mode ##	Causes the Conversion Measurement API to run without delays or noise. ##

--copy-to-download-dir ##	Copy user action data to download directory. ##

--cors-exempt-headers ##	No description ##

--crash ##	Crash flag to force a crash right away. Mainly intended for ensuring crashes are properly recorded by crashpad. ##

--crash-dumps-dir ##	The directory breakpad should store minidumps in. ##

--crash-handler ##	Runs as the Crashpad handler. ##

--crash-loop-before[5] ##	A time\_t. Passed by session\_manager into the Chrome user session, indicating that if Chrome crashes before the indicated time, session\_manager will consider this to be a crash-loop situation and log the user out. Chrome mostly just passes this to crash\_reporter if it crashes. ##

--crash-on-failure ##	When specified to "enable-leak-detection" command-line option, causes the leak detector to cause immediate crash when found leak. ##

--crash-on-hang-threads ##	Comma-separated list of BrowserThreads that cause browser process to crash if the given browser thread is not responsive. UI/IO are the BrowserThreads that are supported. For example: --crash-on-hang-threads=UI:18,IO:18 --> Crash the browser if UI or IO is not responsive for 18 seconds while the other browser thread is responsive. ##

--crash-server-url ##	Server url to upload crash data to. Default is "<https://clients2.google.com/cr/report>" for prod devices. Default is "<https://clients2.google.com/cr/staging_report>" for non prod. ##

--crash-test ##	Causes the browser process to crash on startup. ##

--crashpad-handler ##	A process type (switches::kProcessType) that indicates chrome.exe or setup.exe is being launched as crashpad\_handler. This is only used on Windows. We bundle the handler into chrome.exe on Windows because there is high probability of a "new" .exe being blocked or interfered with by application firewalls, AV software, etc. On other platforms, crashpad\_handler is a standalone executable. ##

--crashpad-handler-pid[10] ##	The process ID of the Crashpad handler. ##

--create-browser-on-startup-for-tests ##	Some platforms like ChromeOS default to empty desktop. Browser tests may need to add this switch so that at least one browser instance is created on startup. TODO(nkostylev): Investigate if this switch could be removed. (<http://crbug.com/148675>) ##

--cros-region ##	Forces CrOS region value. ##

--cros-regions-mode ##	Control regions data load ("" is default). ##

--crosh-command[5] ##	Custom crosh command. ##

--cryptauth-http-host ##	Overrides the default URL for Google APIs (<https://www.googleapis.com>) used by CryptAuth. ##

--cryptauth-v2-devicesync-http-host ##	Overrides the default URL for CryptAuth v2 DeviceSync: <https://cryptauthdevicesync.googleapis.com>. ##

--cryptauth-v2-enrollment-http-host ##	Overrides the default URL for CryptAuth v2 Enrollment: <https://cryptauthenrollment.googleapis.com>. ##

--custom-android-messages-domain ##	No description ##

--custom-devtools-frontend ##	Specifies the http:// endpoint which will be used to serve devtools://devtools/custom/<path> Or a <file://> URL to specify a custom file path to load from for devtools://devtools/bundled/<path> ##

--custom-launcher-page ##	Specifies the chrome-extension:// URL for the contents of an additional page added to the app launcher. ##

--custom\_summary[7] ##	Forces a custom summary to be displayed below the update menu item. ##

--d3d-support[1] ##	No description ##

--d3d11 ##	No description ##

--d3d11-null ##	Special switches for "NULL"/stub driver implementations. ##

--d3d11on12 ##	No description ##

--d3d9 ##	No description ##

--daemon ##	No description ##

--dark ##	No description ##

--data-path ##	Makes Content Shell use the given path for its data directory. ##

--data-reduction-proxy-client-config ##	Uses the encoded ClientConfig instead of fetching one from the config server. This value is always used, regardless of error or expiration. The value should be a base64 encoded binary protobuf. ##

--data-reduction-proxy-config-url ##	The URL from which to retrieve the Data Reduction Proxy configuration. ##

--data-reduction-proxy-experiment ##	The name of a Data Reduction Proxy experiment to run. These experiments are defined by the proxy server. Use --force-fieldtrials for Data Reduction Proxy field trials. ##

--data-reduction-proxy-http-proxies ##	The semicolon-separated list of proxy server URIs to override the list of HTTP proxies returned by the Data Saver API. It is illegal to use |kDataReductionProxy| or |kDataReductionProxyFallback| switch in conjunction with |kDataReductionProxyHttpProxies|. If the URI omits a scheme, then the proxy server scheme defaults to HTTP, and if the port is omitted then the default port for that scheme is used. E.g. "<http://foo.net:80>", "<http://foo.net>", "foo.net:80", and "foo.net" are all equivalent. ##

--data-reduction-proxy-pingback-url ##	No description ##

--data-reduction-proxy-secure-proxy-check-url ##	Sets a secure proxy check URL to test before committing to using the Data Reduction Proxy. Note this check does not go through the Data Reduction Proxy. ##

--data-reduction-proxy-server-experiments-disabled ##	Disables server experiments that may be enabled through field trial. ##

--dbus-stub ##	Forces the stub implementation of D-Bus clients. ##

--de-jelly-screen-width ##	Screen width is useful for debugging. Shipping implementations should detect this. ##

--deadline-to-synchronize-surfaces ##	The default number of the BeginFrames to wait to activate a surface with dependencies. ##

--debug-devtools ##	Run devtools tests in debug mode (not bundled and minified) ##

--debug-enable-frame-toggle ##	Enables a frame context menu item that toggles the frame in and out of glass mode (Windows Vista and up only). ##

--debug-packed-apps ##	Adds debugging entries such as Inspect Element to context menus of packed apps. ##

--debug-print[11] ##	Enables support to debug printing subsystem. ##

--default ##	No description ##

--default-background-color ##	The background color to be used if the page doesn't specify one. Provided as RGBA integer value in hex, e.g. 'ff0000ff' for red or '00000000' for transparent. ##

--default-country-code[7] ##	Default country code to be used for search engine localization. ##

--default-tile-height ##	No description ##

--default-tile-width ##	Sets the tile size used by composited layers. ##

--default-wallpaper-is-oem ##	Indicates that the wallpaper images specified by kAshDefaultWallpaper{Large,Small} are OEM-specific (i.e. they are not downloadable from Google). ##

--default-wallpaper-large ##	Default large wallpaper to use (as path to trusted, non-user-writable JPEG file). ##

--default-wallpaper-small ##	Default small wallpaper to use (as path to trusted, non-user-writable JPEG file). ##

--deny-permission-prompts ##	Prevents permission prompts from appearing by denying instead of showing prompts. ##

--derelict-detection-timeout ##	Time in seconds before a machine at OOBE is considered derelict. ##

--derelict-idle-timeout ##	Time in seconds before a derelict machines starts demo mode. ##

--desktop ##	No description ##

--desktop-window-1080p ##	When present, desktop cast\_shell will create 1080p window (provided display resolution is high enough). Otherwise, cast\_shell defaults to 720p. ##

--deterministic-mode ##	A meta flag. This sets a number of flags which put the browser into deterministic mode where begin frames should be issued over DevToolsProtocol (experimental). ##

--device-management-url ##	Specifies the URL at which to communicate with the device management backend to fetch configuration policies and perform other device tasks. ##

--device-scale-factor[1] ##	Device scale factor passed to certain processes like renderers, etc. ##

--devtools-flags ##	Passes command line parameters to the DevTools front-end. ##

--diagnostics ##	Triggers a plethora of diagnostic modes. ##

--diagnostics-format ##	Sets the output format for diagnostic modes enabled by diagnostics flag. ##

--diagnostics-recovery ##	Tells the diagnostics mode to do the requested recovery step(s). ##

--disable-2d-canvas-clip-aa ##	Disable antialiasing on 2d canvas clips ##

--disable-2d-canvas-image-chromium ##	Disables Canvas2D rendering into a scanout buffer for overlay support. ##

--disable-3d-apis ##	Disables client-visible 3D APIs, in particular WebGL and Pepper 3D. This is controlled by policy and is kept separate from the other enable/disable switches to avoid accidentally regressing the policy support for controlling access to these APIs. ##

--disable-accelerated-2d-canvas ##	Disable gpu-accelerated 2d canvas. ##

--disable-accelerated-mjpeg-decode ##	Disable hardware acceleration of mjpeg decode for captured frame, where available. ##

--disable-accelerated-video-decode ##	Disables hardware acceleration of video decode, where available. ##

--disable-accelerated-video-encode ##	Disables hardware acceleration of video encode, where available. ##

--disable-and-delete-previous-log ##	Disable cros action logging. ##

--disable-angle-features ##	Disables specified comma separated ANGLE features if found. ##

--disable-app-list-dismiss-on-blur ##	If set, the app list will not be dismissed when it loses focus. This is useful when testing the app list or a custom launcher page. It can still be dismissed via the other methods (like the Esc key). ##

--disable-arc-cpu-restriction ##	Prevents any CPU restrictions being set on the ARC container. Only meant to be used by tests as some tests may time out if the ARC container is throttled. ##

--disable-arc-data-wipe ##	Disables android user data wipe on opt out. ##

--disable-arc-opt-in-verification ##	Disables ARC Opt-in verification process and ARC is enabled by default. ##

--disable-audio-output ##	No description ##

--disable-audio-support-for-desktop-share ##	No description ##

--disable-auto-reload ##	Disable auto-reload of error pages. ##

--disable-auto-wpt-origin-isolation ##	Disables the automatic origin isolation of web platform test domains. We normally origin-isolate them for better test coverage, but tests of opt-in origin isolation need to disable this. ##

--disable-avfoundation-overlays[6] ##	Disable use of AVFoundation to draw video content. ##

--disable-ax-menu-list ##	Optionally disable AXMenuList, which makes the internal pop-up menu UI for a select element directly accessible. ##

--disable-back-forward-cache ##	Disables the BackForwardCache feature. ##

--disable-background-media-suspend ##	Do not immediately suspend media in background tabs. ##

--disable-background-networking ##	Disable several subsystems which run network requests in the background. This is for use when doing network performance testing to avoid noise in the measurements. ##

--disable-background-timer-throttling ##	Disable task throttling of timer tasks from background pages. ##

--disable-backgrounding-occluded-windows ##	Disable backgrounding renders for occluded windows. Done for tests to avoid nondeterministic behavior. ##

--disable-backing-store-limit ##	Disable limits on the number of backing stores. Can prevent blinking for users with many windows/tabs and lots of memory. ##

--disable-best-effort-tasks ##	Delays execution of TaskPriority::BEST\_EFFORT tasks until shutdown. ##

--disable-blink-features ##	Disable one or more Blink runtime-enabled features. Use names from runtime\_enabled\_features.json5, separated by commas. Applied after kEnableBlinkFeatures, and after other flags that change these features. ##

--disable-breakpad ##	Disables the crash reporting. ##

--disable-bundled-ppapi-flash ##	Disables the bundled PPAPI version of Flash. ##

--disable-cancel-all-touches[12] ##	Disable CancelAllTouches() function for the implementation on cancel single touches. ##

--disable-canvas-aa ##	Disable antialiasing on 2d canvas. ##

--disable-checker-imaging ##	Disabled deferring all image decodes to the image decode service, ignoring DecodingMode preferences specified on PaintImage. ##

--disable-checking-optimization-guide-user-permissions ##	No description ##

--disable-client-side-phishing-detection ##	Disables the client-side phishing detection feature. Note that even if client-side phishing detection is enabled, it will only be active if the user has opted in to UMA stats and SafeBrowsing is enabled in the preferences. ##

--disable-component-cloud-policy ##	Disables fetching and storing cloud policy for components. ##

--disable-component-extensions-with-background-pages ##	Disable default component extensions with background pages - useful for performance tests where these pages may interfere with perf results. ##

--disable-component-update ##	No description ##

--disable-composited-antialiasing ##	Disables layer-edge anti-aliasing in the compositor. ##

--disable-cookie-encryption ##	Whether cookies stored as part of user profile are encrypted. ##

--disable-crash-reporter ##	Disable crash reporter for headless. It is enabled by default in official builds. ##

--disable-d3d11 ##	Disables use of D3D11. ##

--disable-databases ##	Disables HTML5 DB support. ##

--disable-default-apps ##	Disables installation of default apps on first run. This is used during automated testing. ##

--disable-demo-mode ##	Disables the Chrome OS demo. ##

--disable-dev-shm-usage[13] ##	The /dev/shm partition is too small in certain VM environments, causing Chrome to fail or crash (see <http://crbug.com/715363>). Use this flag to work-around this issue (a temporary directory will always be used to create anonymous shared memory files). ##

--disable-device-disabling ##	If this switch is set, the device cannot be remotely disabled by its owner. ##

--disable-device-discovery-notifications ##	Disables device discovery notifications. ##

--disable-dinosaur-easter-egg ##	Disables the dinosaur easter egg on the offline interstitial. ##

--disable-direct-composition ##	Disables DirectComposition surface. ##

--disable-direct-composition-video-overlays ##	Disables using DirectComposition video overlays, even if hardware overlays are supported. ##

--disable-domain-blocking-for-3d-apis ##	Disable the per-domain blocking for 3D APIs after GPU reset. This switch is intended only for tests. ##

--disable-domain-reliability ##	Disables Domain Reliability Monitoring. ##

--disable-dwm-composition ##	Disables use of DWM composition for top level windows. ##

--disable-encryption-migration ##	Disable encryption migration for user's cryptohome to run latest Arc. ##

--disable-enterprise-policy ##	Disables enterprise policy support. ##

--disable-es3-gl-context ##	Disables use of ES3 backend (use ES2 backend instead). ##

--disable-es3-gl-context-for-testing ##	Disables use of ES3 backend at a lower level, for testing purposes. This isn't guaranteed to work everywhere, so it's test-only. ##

--disable-experimental-accessibility-chromevox-language-switching ##	Disables ChromeVox language switching feature. ##

--disable-experimental-accessibility-chromevox-search-menus ##	Disables ChromeVox search menus feature. ##

--disable-explicit-dma-fences ##	Disable explicit DMA-fences ##

--disable-extensions ##	Disable extensions. ##

--disable-extensions-except ##	Disable extensions except those specified in a comma-separated list. ##

--disable-extensions-file-access-check ##	Disable checking for user opt-in for extensions that want to inject script into file URLs (ie, always allow it). This is used during automated testing. ##

--disable-extensions-http-throttling ##	Disable the net::URLRequestThrottlerManager functionality for requests originating from extensions. ##

--disable-features ##	Comma-separated list of feature names to disable. See also kEnableFeatures. ##

--disable-fetching-hints-at-navigation-start ##	No description ##

--disable-field-trial-config ##	Disable field trial tests configured in fieldtrial\_testing\_config.json. ##

--disable-file-system ##	Disable FileSystem API. ##

--disable-fine-grained-time-zone-detection ##	Disables fine grained time zone detection. ##

--disable-flash-3d ##	Disable 3D inside of flapper. ##

--disable-flash-stage3d ##	Disable Stage3D inside of flapper. ##

--disable-font-subpixel-positioning ##	Force disables font subpixel positioning. This affects the character glyph sharpness, kerning, hinting and layout. ##

--disable-frame-rate-limit ##	Disables begin frame limiting in both cc scheduler and display scheduler. Also implies --disable-gpu-vsync (see //ui/gl/gl\_switches.h). ##

--disable-gaia-services ##	Disables GAIA services such as enrollment and OAuth session restore. Used by 'fake' telemetry login. ##

--disable-gesture-requirement-for-presentation ##	Disable user gesture requirement for presentation. ##

--disable-gl-drawing-for-tests ##	Disables GL drawing operations which produce pixel output. With this the GL output will not be correct but tests will run faster. ##

--disable-gl-error-limit ##	Disable the GL error log limit. ##

--disable-gl-extensions ##	Disables specified comma separated GL Extensions if found. ##

--disable-glsl-translator ##	Disable the GLSL translator. ##

--disable-gpu ##	Disables GPU hardware acceleration. If software renderer is not in place, then the GPU process won't launch. ##

--disable-gpu-compositing ##	Prevent the compositor from using its GPU implementation. ##

--disable-gpu-driver-bug-workarounds ##	Disable workarounds for various GPU driver bugs. ##

--disable-gpu-early-init ##	Disable proactive early init of GPU process. ##

--disable-gpu-memory-buffer-compositor-resources ##	Do not force that all compositor resources be backed by GPU memory buffers. ##

--disable-gpu-memory-buffer-video-frames ##	Disable GpuMemoryBuffer backed VideoFrames. ##

--disable-gpu-process-crash-limit ##	For tests, to disable the limit on the number of times the GPU process may be restarted. ##

--disable-gpu-process-for-dx12-info-collection ##	Disables the non-sandboxed GPU process for DX12 info collection ##

--disable-gpu-program-cache ##	Turn off gpu program caching ##

--disable-gpu-rasterization ##	Disable GPU rasterization, i.e. rasterize on the CPU only. Overrides the kEnableGpuRasterization flag. ##

--disable-gpu-sandbox ##	Disables the GPU process sandbox. ##

--disable-gpu-shader-disk-cache ##	Disables the GPU shader on disk cache. ##

--disable-gpu-vsync ##	Stop the GPU from synchronizing presentation with vblank. ##

--disable-gpu-watchdog ##	Disable the thread that crashes the GPU process if it stops responding to messages. ##

--disable-hang-monitor ##	Suppresses hang monitor dialogs in renderer processes. This may allow slow unload handlers on a page to prevent the tab from closing, but the Task Manager can be used to terminate the offending process in this case. ##

--disable-headless-mode ##	Disables the shell from beginning in headless mode. Tests will then attempt to use the hardware GPU for rendering. This is only followed when kRunWebTests is set. ##

--disable-hid-detection-on-oobe ##	Disables HID-detection OOBE screen. ##

--disable-highres-timer[1] ##	Disable high-resolution timer on Windows. ##

--disable-histogram-customizer ##	Disable the RenderThread's HistogramCustomizer. ##

--disable-hosted-app-shim-creation[6] ##	Disables app shim creation for hosted apps on Mac. ##

--disable-image-animation-resync ##	Disallow image animations to be reset to the beginning to avoid skipping many frames. Only effective if compositor image animations are enabled. ##

--disable-in-process-stack-traces ##	Disables the in-process stack traces. ##

--disable-ios-password-suggestions ##	Disable showing available password credentials in the keyboard accessory view when focused on form fields. ##

--disable-ipc-flooding-protection ##	Disables the IPC flooding protection. It is activated by default. Some javascript functions can be used to flood the browser process with IPC. This protection limits the rate at which they can be used. ##

--disable-javascript-harmony-shipping ##	Disable latest shipping ECMAScript 6 features. ##

--disable-kill-after-bad-ipc ##	Don't kill a child process when it sends a bad IPC message. Apart from testing, it is a bad idea from a security perspective to enable this switch. ##

--disable-layer-tree-host-memory-pressure ##	Disables LayerTreeHost::OnMemoryPressure ##

--disable-lcd-text ##	Disables LCD text. ##

--disable-legacy-window[1] ##	Disable the Legacy Window which corresponds to the size of the WebContents. ##

--disable-local-storage ##	Disable LocalStorage. ##

--disable-logging ##	Force logging to be disabled. Logging is enabled by default in debug builds. ##

--disable-logging-redirect[5] ##	Disables logging redirect for testing. ##

--disable-login-animations ##	Avoid doing expensive animations upon login. ##

--disable-login-screen-apps[5] ##	Disables apps on the login screen. By default, they are allowed and can be installed through policy. ##

--disable-low-end-device-mode ##	Force disabling of low-end device mode when set. ##

--disable-low-latency-dxva ##	Disables using CODECAPI\_AVLowLatencyMode when creating DXVA decoders. ##

--disable-low-res-tiling ##	When using CPU rasterizing disable low resolution tiling. This uses less power, particularly during animations, but more white may be seen during fast scrolling especially on slower devices. ##

--disable-mac-overlays[6] ##	Fall back to using CAOpenGLLayers display content, instead of the IOSurface based overlay display path. ##

--disable-machine-cert-request ##	Disables requests for an enterprise machine certificate during attestation. ##

--disable-main-frame-before-activation ##	Disables sending the next BeginMainFrame before the previous commit activates. Overrides the kEnableMainFrameBeforeActivation flag. ##

--disable-media-session-api[7] ##	Disable Media Session API ##

--disable-metal-test-shaders ##	No description ##

--disable-modal-animations[6] ##	Disable animations for showing and hiding modal dialogs. ##

--disable-mojo-renderer ##	Rather than use the renderer hosted remotely in the media service, fall back to the default renderer within content\_renderer. Does not change the behavior of the media service. ##

--disable-multi-display-layout ##	Disables the multiple display layout UI. ##

--disable-namespace-sandbox ##	Disables usage of the namespace sandbox. ##

--disable-new-content-rendering-timeout ##	Disables clearing the rendering output of a renderer when it didn't commit new output for a while after a top-frame navigation. ##

--disable-notifications ##	Disables the Web Notification and the Push APIs. ##

--disable-nv12-dxgi-video ##	Disables the video decoder from drawing to an NV12 textures instead of ARGB. ##

--disable-offer-store-unmasked-wallet-cards ##	Force hiding the local save checkbox in the autofill dialog box for getting the full credit card number for a wallet card. The card will never be stored locally. ##

--disable-oop-rasterization ##	Disables OOP rasterization. Takes precedence over the enable flag. ##

--disable-oopr-debug-crash-dump[7] ##	Disables debug crash dumps for OOPR. ##

--disable-oor-cors ##	Disable OOR-CORS in child processes regardless of the base::Feature flag. ##

--disable-origin-trial-controlled-blink-features ##	Disables all RuntimeEnabledFeatures that can be enabled via OriginTrials. ##

--disable-overscroll-edge-effect[7] ##	Disable overscroll edge effects like those found in Android views. ##

--disable-partial-raster ##	Disable partial raster in the renderer. Disabling this switch also disables the use of persistent gpu memory buffers. ##

--disable-pepper-3d ##	Disable Pepper3D. ##

--disable-pepper-3d-image-chromium ##	Disable Image Chromium for Pepper 3d. ##

--disable-per-user-timezone ##	Disables per-user timezone. ##

--disable-permissions-api ##	Disables the Permissions API. ##

--disable-pinch ##	Disables compositor-accelerated touch-screen pinch gestures. ##

--disable-pnacl-crash-throttling ##	Disables crash throttling for Portable Native Client. ##

--disable-popup-blocking ##	Disables pop-up blocking. ##

--disable-prefer-compositing-to-lcd-text ##	Disable the creation of compositing layers when it would prevent LCD text. ##

--disable-presentation-api ##	Disables the Presentation API. ##

--disable-print-preview ##	Disables print preview (For testing, and for users who don't like us. :[ ) ##

--disable-prompt-on-repost ##	Normally when the user attempts to navigate to a page that was the result of a post we prompt to make sure they want to. This switch may be used to disable that check. This switch is used during automated testing. ##

--disable-pull-to-refresh-effect[7] ##	Disable the pull-to-refresh effect when vertically overscrolling content. ##

--disable-pushstate-throttle ##	Disables throttling of history.pushState/replaceState calls. ##

--disable-reading-from-canvas ##	Taints all <canvas> elements, regardless of origin. ##

--disable-remote-core-animation[6] ##	Disable use of cross-process CALayers to display content directly from the GPU process on Mac. ##

--disable-remote-fonts ##	Disables remote web font support. SVG font should always work whether this option is specified or not. ##

--disable-remote-playback-api ##	Disables the RemotePlayback API. ##

--disable-renderer-accessibility ##	Turns off the accessibility in the renderer. ##

--disable-renderer-backgrounding ##	Prevent renderer process backgrounding when set. ##

--disable-resource-scheduler ##	Whether the ResourceScheduler is disabled. Note this is only useful for C++ Headless embedders who need to implement their own resource scheduling. ##

--disable-rgba-4444-textures ##	Disables RGBA\_4444 textures. ##

--disable-rollback-option ##	Disables rollback option on reset screen. ##

--disable-rtc-smoothness-algorithm ##	Disables the new rendering algorithm for webrtc, which is designed to improve the rendering smoothness. ##

--disable-running-as-system-compositor ##	Disable running as system compositor. ##

--disable-screen-orientation-lock[7] ##	Disable the locking feature of the screen orientation API. ##

--disable-scroll-to-text-fragment ##	This switch disables the ScrollToTextFragment feature. ##

--disable-search-geolocation-disclosure ##	Disables showing the search geolocation disclosure UI. Used for perf testing. ##

--disable-seccomp-filter-sandbox ##	Disable the seccomp filter sandbox (seccomp-bpf) (Linux only). ##

--disable-setuid-sandbox ##	Disable the setuid sandbox (Linux only). ##

--disable-shader-name-hashing ##	Turn off user-defined name hashing in shaders. ##

--disable-shared-workers ##	Disable shared workers. ##

--disable-signin-frame-client-certs ##	Disables client certificate authentication on the sign-in frame on the Chrome OS sign-in profile. TODO(https://crbug.com/844022): Remove this flag when reaching endpoints that request client certs does not hang anymore when there is no system token yet. ##

--disable-signin-scoped-device-id ##	Disables sending signin scoped device id to LSO with refresh token request. ##

--disable-site-isolation-for-policy[7] ##	Just like kDisableSiteIsolation, but doesn't show the "stability and security will suffer" butter bar warning. ##

--disable-site-isolation-trials ##	Disables site isolation. Note that the opt-in (to site-per-process, isolate-origins, etc.) via enterprise policy and/or cmdline takes precedence over the kDisableSiteIsolation switch (i.e. the opt-in takes effect despite potential presence of kDisableSiteIsolation switch). Note that for historic reasons the name of the switch misleadingly mentions "trials", but the switch also disables the default site isolation that ships on desktop since M67. The name of the switch is preserved for backcompatibility of chrome://flags. ##

--disable-skia-runtime-opts ##	Do not use runtime-detected high-end CPU optimizations in Skia. This is useful for forcing a baseline code path for e.g. web tests. ##

--disable-smooth-scrolling ##	Disable smooth scrolling for testing. ##

--disable-software-compositing-fallback ##	For tests, to disable falling back to software compositing if the GPU Process has crashed, and reached the GPU Process crash limit. ##

--disable-software-rasterizer ##	Disables the use of a 3D software rasterizer. ##

--disable-software-video-decoders ##	No description ##

--disable-speech-api ##	Disables the Web Speech API (both speech recognition and synthesis). ##

--disable-speech-synthesis-api ##	Disables the speech synthesis part of Web Speech API. ##

--disable-sync ##	Disables syncing browser data to a Google Account. ##

--disable-sync-types ##	Disables syncing one or more sync data types that are on by default. See sync/base/model\_type.h for possible types. Types should be comma separated, and follow the naming convention for string representation of model types, e.g.: --disable-synctypes='Typed URLs, Bookmarks, Autofill Profiles' ##

--disable-system-font-check ##	Disables the check for the system font when specified. ##

--disable-test-root-certs ##	Disables adding the test certs in the network process. ##

--disable-third-party-keyboard-workaround ##	Disables the 3rd party keyboard omnibox workaround. ##

--disable-threaded-animation ##	No description ##

--disable-threaded-compositing ##	Disable multithreaded GPU compositing of web content. ##

--disable-threaded-scrolling ##	Disable multithreaded, compositor scrolling of web content. ##

--disable-timeouts-for-profiling[7] ##	Disable timeouts that may cause the browser to die when running slowly. This is useful if running with profiling (such as debug malloc). ##

--disable-touch-adjustment ##	Disables touch adjustment. ##

--disable-touch-drag-drop ##	Disables touch event based drag and drop. ##

--disable-usb-keyboard-detect[1] ##	Disables the USB keyboard detection for blocking the OSK on Win8+. ##

--disable-v8-idle-tasks ##	Disable V8 idle tasks. ##

--disable-video-capture-use-gpu-memory-buffer ##	This is for the same feature controlled by kVideoCaptureUseGpuMemoryBuffer. kVideoCaptureUseGpuMemoryBuffer is settled by chromeos overlays. This flag is necessary to overwrite the settings via chrome:// flag. The behavior of chrome://flag#zero-copy-video-capture is as follows; Default : Respect chromeos overlays settings. Enabled : Force to enable kVideoCaptureUseGpuMemoryBuffer. Disabled : Force to disable kVideoCaptureUseGpuMemoryBuffer. ##

--disable-volume-adjust-sound ##	Disables volume adjust sound. ##

--disable-vsync-for-tests ##	No description ##

--disable-vulkan-fallback-to-gl-for-testing ##	Disables falling back to GL based hardware rendering if initializing Vulkan fails. This is to allow tests to catch regressions in Vulkan. ##

--disable-vulkan-surface ##	Disables VK\_KHR\_surface extension. Instead of using swapchain, bitblt will be used for present render result on screen. ##

--disable-wake-on-wifi ##	Disables wake on wifi features. ##

--disable-web-security ##	Don't enforce the same-origin policy. (Used by people testing their sites.) ##

--disable-webgl ##	Disable all versions of WebGL. ##

--disable-webgl-image-chromium ##	Disables WebGL rendering into a scanout buffer for overlay support. ##

--disable-webgl2 ##	Disable WebGL2. ##

--disable-webrtc-encryption ##	Disables encryption of RTP Media for WebRTC. When Chrome embeds Content, it ignores this switch on its stable and beta channels. ##

--disable-webrtc-hw-decoding ##	Disables HW decode acceleration for WebRTC. ##

--disable-webrtc-hw-encoding ##	Disables HW encode acceleration for WebRTC. ##

--disable-windows10-custom-titlebar[1] ##	Disables custom-drawing the window titlebar on Windows 10. ##

--disable-yuv-image-decoding ##	Disable YUV image decoding for those formats and cases where it's supported. Has no effect unless GPU rasterization is enabled. ##

--disable-yuv420-biplanar ##	Disable a NV12 format buffer allocation with gfx::BufferUsage::SCANOUT\_CPU\_READ\_WRITE usage. ##

--disable-zero-browsers-open-for-tests ##	Some tests seem to require the application to close when the last browser window is closed. Thus, we need a switch to force this behavior for ChromeOS Aura, disable "zero window mode". TODO(pkotwicz): Investigate if this bug can be removed. (<http://crbug.com/119175>) ##

--disable-zero-copy ##	Disable rasterizer that writes directly to GPU memory associated with tiles. ##

--disable-zero-copy-dxgi-video ##	Disable the video decoder from drawing directly to a texture. ##

--disabled ##	No description ##

--disallow-non-exact-resource-reuse ##	Disable re-use of non-exact resources to fulfill ResourcePool requests. Intended only for use in layout or pixel tests to reduce noise. ##

--discoverability ##	No description ##

--disk-cache-dir ##	Use a specific disk cache location, rather than one derived from the UserDatadir. ##

--disk-cache-size ##	Forces the maximum disk space to be used by the disk cache, in bytes. ##

--display ##	Which X11 display to connect to. Emulates the GTK+ "--display=" command line argument. ##

--dmg-device[6] ##	When switches::kProcessType is switches::kRelauncherProcess, if this switch is also present, the relauncher process will unmount and eject a mounted disk image and move its disk image file to the trash. The argument's value must be a BSD device name of the form "diskN" or "diskNsM". ##

--dns-log-details ##	No description ##

--document-user-activation-required ##	Autoplay policy that requires a document user activation. ##

--dom-automation ##	Specifies if the |DOMAutomationController| needs to be bound in the renderer. This binding happens on per-frame basis and hence can potentially be a performance bottleneck. One should only enable it when automating dom based tests. ##

--dont-require-litepage-redirect-infobar ##	Do not require the user notification InfoBar to be shown before triggering a Lite Page Redirect preview. ##

--double-buffer-compositing ##	Sets the number of max pending frames in the GL buffer queue to 1. ##

--draw-view-bounds-rects ##	Draws a semitransparent rect to indicate the bounds of each view. ##

--dump-blink-runtime-call-stats ##	Logs Runtime Call Stats for Blink. --single-process also needs to be used along with this for the stats to be logged. ##

--dump-browser-histograms ##	Requests that a running browser process dump its collected histograms to a given file. The file is overwritten if it exists. ##

--dump-dom ##	Instructs headless\_shell to print document.body.innerHTML to stdout. ##

--dump-raw-logs ##	Dump the raw logs to a file with the same base name as the executable. The dumped file is a raw protobuf and has a "pb" extension. WARNING: this switch is used by internal test systems. Be careful when making changes. ##

--easy-unlock-app-path ##	Overrides the path of Easy Unlock component app. ##

--edge-touch-filtering[12] ##	Tells Chrome to do edge touch filtering. Useful for convertible tablet. ##

--egl ##	No description ##

--elevate ##	No description ##

--elevated ##	Identify that the process is already supposed to be elevated, so that we don't try again. ##

--embedded-extension-options ##	Enables extension options to be embedded in chrome://extensions rather than a new tab. ##

--emulate-shader-precision ##	Emulate ESSL lowp and mediump float precisions by mutating the shaders to round intermediate values in ANGLE. ##

--enable-accelerated-2d-canvas ##	Enable accelerated 2D canvas. ##

--enable-accessibility-object-model ##	Enable the experimental Accessibility Object Model APIs in development. ##

--enable-accessibility-tab-switcher[7] ##	Enable the accessibility tab switcher. ##

--enable-adaptive-selection-handle-orientation[7] ##	Enable inverting of selection handles so that they are not clipped by the viewport boundaries. ##

--enable-aggressive-domstorage-flushing ##	Enable the aggressive flushing of DOM Storage to minimize data loss. ##

--enable-angle-features ##	ANGLE features are defined per-backend in third\_party/angle/include/platform Enables specified comma separated ANGLE features if found. ##

--enable-app-list ##	If set, the app list will be enabled as if enabled from CWS. ##

--enable-arc ##	DEPRECATED. Please use --arc-availability=officially-supported. Enables starting the ARC instance upon session start. ##

--enable-arcvm ##	Enables ARC VM. ##

--enable-audio-debug-recordings-from-extension ##	If the WebRTC logging private API is active, enables audio debug recordings. ##

--enable-auto-reload ##	Enable auto-reload of error pages. ##

--enable-automation ##	Enable indication that browser is controlled by automation. ##

--enable-background-tracing ##	Enables background and upload trace to trace-upload-url. Trigger rules are pass as an argument. ##

--enable-begin-frame-control ##	Whether or not begin frames should be issued over DevToolsProtocol (experimental). ##

--enable-benchmarking ##	TODO(asvitkine): Consider removing or renaming this functionality. Enables the benchmarking extensions. ##

--enable-ble-advertising-in-apps ##	Enable BLE Advertisiing in apps. ##

--enable-blink-features ##	Enable one or more Blink runtime-enabled features. Use names from runtime\_enabled\_features.json5, separated by commas. Applied before kDisableBlinkFeatures, and after other flags that change these features. ##

--enable-blink-test-features ##	Enables blink runtime enabled features with status:"test" or status:"experimental", which are enabled when running web tests. ##

--enable-bluetooth-spp-in-serial-api ##	Enable serial communication for SPP devices. ##

--enable-bookmark-undo ##	Enables the multi-level undo system for bookmarks. ##

--enable-caret-browsing ##	Enable native caret browsing, in which a moveable cursor is placed on a web page, allowing a user to select and navigate through non-editable text using just a keyboard. See <https://crbug.com/977390> for links to i2i. ##

--enable-cast-receiver ##	Enables the Cast Receiver. ##

--enable-cast-streaming-receiver ##	No description ##

--enable-chrome-browser-cloud-management ##	Enables Chrome Browser Cloud Management flow. ##

--enable-chrome-tracing-computation ##	Enable the tracing service. ##

--enable-cloud-print-proxy ##	This applies only when the process type is "service". Enables the Cloud Print Proxy component within the service process. ##

--enable-consumer-kiosk ##	Enables consumer kiosk mode for Chrome OS. ##

--enable-crash-reporter ##	Enable crash reporter for headless. ##

--enable-crash-reporter-for-testing[14] ##	Used for turning on Breakpad crash reporting in a debug environment where crash reporting is typically compiled but disabled. ##

--enable-cros-action-recorder ##	If set, the CrOSActionRecorder will be enabled which will record some user actions on device. ##

--enable-crx-hash-check ##	Enable package hash check: the .crx file sha256 hash sum should be equal to the one received from update manifest. ##

--enable-data-reduction-proxy-bypass-warning ##	Enable the data reduction proxy bypass warning. ##

--enable-data-reduction-proxy-force-pingback ##	Enables sending a pageload metrics pingback after every page load. ##

--enable-data-reduction-proxy-savings-promo ##	Enables a 1 MB savings promo for the data reduction proxy. ##

--enable-dawn-backend-validation ##	Enable validation layers in Dawn backends. ##

--enable-de-jelly ##	Experimental de-jelly support. ##

--enable-defer-all-script-without-optimization-hints ##	Allows defer script preview on all https pages even if optimization hints are missing for that webpage. ##

--enable-device-discovery-notifications ##	Enable device discovery notifications. ##

--enable-dim-shelf ##	Enables Shelf Dimming for ChromeOS. ##

--enable-direct-composition-video-overlays ##	Enables using DirectComposition video overlays, even if hardware overlays aren't supported. ##

--enable-distillability-service ##	No description ##

--enable-dom-distiller ##	No description ##

--enable-domain-reliability ##	Enables Domain Reliability Monitoring. ##

--enable-encryption-migration ##	Enables encryption migration for user's cryptohome to run latest Arc. ##

--enable-encryption-selection[9] ##	Enables the feature of allowing the user to disable the backend via a setting. ##

--enable-enterprise-policy ##	Enables enterprise policy support. ##

--enable-exclusive-audio[1] ##	Use exclusive mode audio streaming for Windows Vista and higher. Leads to lower latencies for audio streams which uses the AudioParameters::AUDIO\_PCM\_LOW\_LATENCY audio path. See <http://msdn.microsoft.com/en-us/library/windows/desktop/dd370844.aspx> for details. ##

--enable-experimental-accessibility-autoclick ##	Shows additional automatic click features that haven't launched yet. ##

--enable-experimental-accessibility-chromevox-annotations ##	Enables annotations feature that hasn't launched yet. ##

--enable-experimental-accessibility-chromevox-tutorial ##	Enables interactive tutorial for ChromeVox. ##

--enable-experimental-accessibility-labels-debugging ##	Enables support for visually debugging the accessibility labels feature, which provides images descriptions for screen reader users. ##

--enable-experimental-accessibility-language-detection ##	Enables language detection on in-page text content which is then exposed to assistive technology such as screen readers. ##

--enable-experimental-accessibility-language-detection-dynamic ##	Enables language detection for dynamic content which is then exposed to assistive technology such as screen readers. ##

--enable-experimental-accessibility-switch-access ##	Shows setting to enable Switch Access before it has launched. ##

--enable-experimental-accessibility-switch-access-text ##	Enables in progress Switch Access features for text input. ##

--enable-experimental-cookie-features ##	Flag that turns on a group of experimental/newly added cookie-related features together, as a convenience for e.g. testing, to avoid having to set multiple switches individually which may be error-prone (not to mention tedious). There is not a corresponding switch to disable all these features, because that is discouraged, and for testing purposes you'd need to switch them off individually to identify the problematic feature anyway. At present this turns on: net::features::kCookiesWithoutSameSiteMustBeSecure net::features::kSameSiteByDefaultCookies net::features::kSameSiteDefaultChecksMethodRigorously It will soon also turn on: content\_settings::kImprovedCookieControls content\_settings::kImprovedCookieControlsForThirdPartyCookieBlocking net::features::kSchemefulSameSite ##

--enable-experimental-extension-apis ##	Enables extension APIs that are in development. ##

--enable-experimental-ui-automation[1] ##	Enables UI Automation platform API in addition to the IAccessible API. ##

--enable-experimental-web-platform-features ##	Enables Web Platform features that are in development. ##

--enable-experimental-webassembly-features ##	Enables experimental WebAssembly features. ##

--enable-extension-activity-log-testing ##	No description ##

--enable-extension-activity-logging ##	Enables logging for extension activity. ##

--enable-extension-assets-sharing ##	Enables sharing assets for installed default apps. ##

--enable-features ##	Comma-separated list of feature names to enable. See also kDisableFeatures. ##

--enable-font-antialiasing ##	Enable font antialiasing for pixel tests. ##

--enable-ftp ##	Enables support for FTP URLs. See <https://crbug.com/333943>. ##

--enable-gamepad-button-axis-events ##	Enables gamepadbuttondown, gamepadbuttonup, gamepadbuttonchange, gamepadaxismove non-standard gamepad events. ##

--enable-gpu-benchmarking ##	Enables the GPU benchmarking extension ##

--enable-gpu-blocked-time ##	Enables measures of how long GPU Main Thread was blocked between SwapBuffers ##

--enable-gpu-client-logging ##	Enable GPU client logging. ##

--enable-gpu-client-tracing ##	Enables TRACE for GL calls in the renderer. ##

--enable-gpu-command-logging ##	Turn on Logging GPU commands. ##

--enable-gpu-debugging ##	Turn on Calling GL Error after every command. ##

--enable-gpu-driver-debug-logging ##	Enable logging of GPU driver debug messages. ##

--enable-gpu-memory-buffer-compositor-resources ##	Specify that all compositor resources should be backed by GPU memory buffers. ##

--enable-gpu-memory-buffer-video-frames ##	Enable GpuMemoryBuffer backed VideoFrames. ##

--enable-gpu-rasterization ##	Allow heuristics to determine when a layer tile should be drawn with the Skia GPU backend. Only valid with GPU accelerated compositing. ##

--enable-gpu-service-logging ##	Turns on GPU logging (debug build only). ##

--enable-gpu-service-tracing ##	Turns on calling TRACE for every GL call. ##

--enable-hardware-overlays ##	Enable compositing individual elements via hardware overlays when permitted by device. Setting the flag to "single-fullscreen" will try to promote a single fullscreen overlay and use it as main framebuffer where possible. ##

--enable-hosted-app-quit-notification[6] ##	Shows a notification when quitting Chrome with hosted apps running. Default behavior is to also quit all hosted apps. ##

--enable-houdini ##	Enables the use of 32-bit Houdini library for ARM binary translation. ##

--enable-houdini64 ##	Enables the use of 64-bit Houdini library for ARM binary translation. ##

--enable-hung-renderer-infobar[7] ##	Enables a hung renderer InfoBar allowing the user to close or wait on unresponsive web content. ##

--enable-idle-tracing[7] ##	Adds additional thread idle time information into the trace event output. ##

--enable-input ##	Enables input event handling by the window manager. ##

--enable-ios-handoff-to-other-devices ##	Enables support for Handoff from Chrome on iOS to the default browser of other Apple devices. ##

--enable-lcd-text ##	Enables LCD text. ##

--enable-leak-detection ##	Enables the leak detection of loading webpages. This allows us to check whether or not reloading a webpage releases web-related objects correctly. ##

--enable-local-file-accesses ##	Enable file accesses. It should not be enabled for most Cast devices. ##

--enable-local-sync-backend ##	Enabled the local sync backend implemented by the LoopbackServer. ##

--enable-logging ##	Controls whether console logging is enabled and optionally configures where it's routed. ##

--enable-longpress-drag-selection[7] ##	Enable drag manipulation of longpress-triggered text selections. ##

--enable-low-end-device-mode ##	Force low-end device mode when set. ##

--enable-low-res-tiling ##	When using CPU rasterizing generate low resolution tiling. Low res tiles may be displayed during fast scrolls especially on slower devices. ##

--enable-main-frame-before-activation ##	Enables sending the next BeginMainFrame before the previous commit activates. ##

--enable-nacl ##	Runs the Native Client inside the renderer process and enables GPU plugin (internally adds lEnableGpuPlugin to the command line). ##

--enable-nacl-debug ##	Enables debugging via RSP over a socket. ##

--enable-nacl-nonsfi-mode ##	Enables Non-SFI mode, in which programs can be run without NaCl's SFI sandbox. ##

--enable-native-gpu-memory-buffers ##	Enable native CPU-mappable GPU memory buffer support on Linux. ##

--enable-natural-scroll-default ##	Enables natural scroll by default. ##

--enable-ndk-translation ##	Enables the use of 32-bit NDK translation library for ARM binary translation. ##

--enable-ndk-translation64 ##	Enables the use of 64-bit NDK translation library for ARM binary translation. ##

--enable-net-benchmarking ##	Enables the network-related benchmarking extensions. ##

--enable-network-information-downlink-max ##	Enables the type, downlinkMax attributes of the NetInfo API. Also, enables triggering of change attribute of the NetInfo API when there is a change in the connection type. ##

--enable-new-app-menu-icon[15] ##	No description ##

--enable-ntp-search-engine-country-detection ##	Enables using the default search engine country to show country specific popular sites on the NTP. ##

--enable-offer-store-unmasked-wallet-cards ##	Force showing the local save checkbox in the autofill dialog box for getting the full credit card number for a wallet card. ##

--enable-oop-rasterization ##	Turns on out of process raster for the renderer whenever gpu raster would have been used. Enables the chromium\_raster\_transport extension. ##

--enable-pepper-testing ##	Enables the testing interface for PPAPI. ##

--enable-pixel-output-in-tests ##	Forces tests to produce pixel output when they normally wouldn't. ##

--enable-plugin-placeholder-testing ##	Enables testing features of the Plugin Placeholder. For internal use only. ##

--enable-potentially-annoying-security-features ##	Enables a number of potentially annoying security features (strict mixed content mode, powerful feature restrictions, etc.) ##

--enable-precise-memory-info ##	Make the values returned to window.performance.memory more granular and more up to date in shared worker. Without this flag, the memory information is still available, but it is bucketized and updated less frequently. This flag also applies to workers. ##

--enable-prefer-compositing-to-lcd-text ##	Enable the creation of compositing layers when it would prevent LCD text. ##

--enable-profile-shortcut-manager[1] ##	Force-enables the profile shortcut manager. This is needed for tests since they use a custom-user-data-dir which disables this. ##

--enable-protected-video-buffers[16] ##	Enables protected buffers for encrypted video streams. ##

--enable-reached-code-profiler[7] ##	Enables the reached code profiler that samples all threads in all processes to determine which functions are almost never executed. ##

--enable-reporting[17] ##	Enables metrics and crash reporting. ##

--enable-request-tablet-site ##	Enables request of tablet site (via user agent override). ##

--enable-rgba-4444-textures ##	Enables RGBA\_4444 textures. ##

--enable-sandbox-logging[6] ##	Cause the OS X sandbox write to syslog every time an access to a resource is denied by the sandbox. ##

--enable-service-binary-launcher ##	If true the ServiceProcessLauncher is used to launch services. This allows for service binaries to be loaded rather than using the utility process. This is only useful for tests. ##

--enable-service-manager-tracing ##	Enable the tracing service. ##

--enable-sgi-video-sync ##	Enable use of the SGI\_video\_sync extension, which can have driver/sandbox/window manager compatibility issues. ##

--enable-skia-benchmarking ##	Enables the Skia benchmarking extension. ##

--enable-smooth-scrolling ##	On platforms that support it, enables smooth scroll animation. ##

--enable-spatial-navigation ##	Enable spatial navigation ##

--enable-spdy-proxy-auth ##	Enable the data reduction proxy. ##

--enable-speech-dispatcher[17] ##	Allows sending text-to-speech requests to speech-dispatcher, a common Linux speech service. Because it's buggy, the user must explicitly enable it so that visiting a random webpage can't cause instability. ##

--enable-spotlight-actions ##	Enables the Spotlight actions. ##

--enable-stats-collection-bindings ##	Specifies if the |StatsCollectionController| needs to be bound in the renderer. This binding happens on per-frame basis and hence can potentially be a performance bottleneck. One should only enable it when running a test that needs to access the provided statistics. ##

--enable-strict-mixed-content-checking ##	Blocks all insecure requests from secure contexts, and prevents the user from overriding that decision. ##

--enable-strict-powerful-feature-restrictions ##	Blocks insecure usage of a number of powerful features (device orientation, for example) that we haven't yet deprecated for the web at large. ##

--enable-swap-buffers-with-bounds ##	Enables SwapBuffersWithBounds if it is supported. ##

--enable-switch-access-point-scanning ##	Enables Switch Access point scanning. This feature hasn't launched yet. ##

--enable-tablet-form-factor ##	Enables tablet form factor. ##

--enable-third-party-keyboard-workaround ##	Enables the 3rd party keyboard omnibox workaround. ##

--enable-thread-instruction-count[10] ##	Controls whether or not retired instruction counts are surfaced for threads in trace events on Linux. This flag requires the BPF sandbox to be disabled. ##

--enable-threaded-compositing ##	Enabled threaded compositing for web tests. ##

--enable-threaded-texture-mailboxes ##	Simulates shared textures when share groups are not available. Not available everywhere. ##

--enable-top-drag-gesture ##	Whether to enable detection and dispatch of a 'drag from the top' gesture. ##

--enable-touch-calibration-setting ##	Enables the touch calibration option in MD settings UI for valid touch displays. ##

--enable-touch-drag-drop ##	Enables touch event based drag and drop. ##

--enable-touchpad-three-finger-click ##	Enables touchpad three-finger-click as middle button. ##

--enable-touchview ##	If the flag is present, it indicates 1) the device has accelerometer and 2) the device is a convertible device or a tablet device (thus is capable of entering tablet mode). If this flag is not set, then the device is not capable of entering tablet mode. For example, Samus has accelerometer, but is not a covertible or tablet, thus doesn't have this flag set, thus can't enter tablet mode. ##

--enable-trace-app-source ##	Pass launch source to platform apps. ##

--enable-tracing ##	Enable tracing during the execution of browser tests. ##

--enable-tracing-output ##	The filename to write the output of the test tracing to. ##

--enable-ui-devtools ##	Enables DevTools server for UI (mus, ash, etc). Value should be the port the server is started on. Default port is 9223. ##

--enable-unsafe-webgpu ##	No description ##

--enable-use-zoom-for-dsf ##	Enable the mode that uses zooming to implement device scale factor behavior. ##

--enable-user-metrics[6] ##	Enable user metrics from within the installer. ##

--enable-usermedia-screen-capturing ##	Enable screen capturing support for MediaStream API. ##

--enable-viewport ##	Enables the use of the @viewport CSS rule, which allows pages to control aspects of their own layout. This also turns on touch-screen pinch gestures. ##

--enable-virtual-keyboard ##	No description ##

--enable-viz-devtools ##	Enables inspecting Viz Display Compositor objects. Default port is 9229. For local inspection use chrome://inspect#other ##

--enable-viz-hit-test-debug ##	Enables hit-test debug logging. ##

--enable-vtune-support ##	Enable the Vtune profiler support. ##

--enable-wayland-ime ##	Try to enable wayland input method editor. ##

--enable-wayland-server ##	Enable the wayland server. ##

--enable-web-auth-deprecated-mojo-testing-api ##	Enable the WebAuthn Mojo Testing API. This is a way to interact with the virtual authenticator environment through a mojo interface and is supported only to run web-platform-tests on content shell. Removal of this deprecated API is blocked on crbug.com/937369. ##

--enable-webgl-draft-extensions ##	Enables WebGL extensions not yet approved by the community. ##

--enable-webgl-image-chromium ##	Enables WebGL rendering into a scanout buffer for overlay support. ##

--enable-webgl2-compute-context ##	Enable WebGL2 Compute context. ##

--enable-webrtc-srtp-aes-gcm ##	Enables negotiation of GCM cipher suites from RFC 7714 for SRTP in WebRTC. See <https://tools.ietf.org/html/rfc7714> for further information. ##

--enable-webrtc-srtp-encrypted-headers ##	Enables negotiation of encrypted header extensions from RFC 6904 for SRTP in WebRTC. See <https://tools.ietf.org/html/rfc6904> for further information. TODO(https://crbug.com/954201): Remove this. ##

--enable-webrtc-stun-origin ##	Enables Origin header in Stun messages for WebRTC. ##

--enable-widevine ##	No description ##

--enable-win7-webrtc-hw-h264-decoding[1] ##	Enables H264 HW decode acceleration for WebRtc on Win 7. ##

--enable-zero-copy ##	Enable rasterizer that writes directly to GPU memory associated with tiles. ##

--enabled ##	No description ##

--encode-binary ##	Encode binary web test results (images, audio) using base64. ##

--enforce ##	No description ##

--enforce-gl-minimums ##	Enforce GL minimums. ##

--enforce-vulkan-protected-memory ##	Forces to use protected memory for vulkan compositing. ##

--enforce-webrtc-ip-permission-check ##	Enforce IP Permission check. TODO(guoweis): Remove this once the feature is not under finch and becomes the default. ##

--enforce\_strict ##	No description ##

--ensure-forced-color-profile ##	Crash the browser at startup if the display's color profile does not match the forced color profile. This is necessary on Mac because Chrome's pixel output is always subject to the color conversion performed by the operating system. On all other platforms, this is a no-op. ##

--enterprise-disable-arc ##	Disables ARC for managed accounts. ##

--enterprise-enable-forced-re-enrollment ##	Whether to enable forced enterprise re-enrollment. ##

--enterprise-enable-initial-enrollment ##	Whether to enable initial enterprise enrollment. ##

--enterprise-enable-psm ##	Whether to enable private set membership queries. ##

--enterprise-enable-zero-touch-enrollment ##	Enables the zero-touch enterprise enrollment flow. ##

--enterprise-enrollment-initial-modulus ##	Power of the power-of-2 initial modulus that will be used by the auto-enrollment client. E.g. "4" means the modulus will be 2^4 = 16. ##

--enterprise-enrollment-modulus-limit ##	Power of the power-of-2 maximum modulus that will be used by the auto-enrollment client. ##

--evaluate-type ##	No description ##

--evaluate\_capability ##	No description ##

--explicitly-allowed-ports ##	Allows overriding the list of restricted ports by passing a comma-separated list of port numbers. ##

--export-tagged-pdf ##	If enabled, generate a tagged (accessible) file when printing to PDF. The plan is for this to go away once tagged PDFs become the default. See <https://crbug.com/607777> ##

--expose-internals-for-testing ##	Exposes the window.internals object to JavaScript for interactive development and debugging of web tests that rely on it. ##

--extension-content-verification ##	Name of the command line flag to force content verification to be on in one of various modes. ##

--extension-process ##	Marks a renderer as extension process. ##

--extension-updater-test-request ##	No description ##

--extensions-install-verification ##	Turns on extension install verification if it would not otherwise have been turned on. ##

--extensions-not-webstore ##	Specifies a comma-separated list of extension ids that should be forced to be treated as not from the webstore when doing install verification. ##

--extensions-on-chrome-urls ##	Enables extensions running scripts on chrome:// URLs. Extensions still need to explicitly request access to chrome:// URLs in the manifest. ##

--external-metrics-collection-interval ##	Interval in seconds between Chrome reading external metrics from /var/lib/metrics/uma-events. ##

--extra-search-query-params ##	Additional query params to insert in the search and instant URLs. Useful for testing. ##

--fail-audio-stream-creation ##	Causes the AudioManager to fail creating audio streams. Used when testing various failure cases. ##

--fake-drivefs-launcher-chroot-path ##	An absolute path to the chroot hosting the DriveFS to use. This is only used when running on Linux, i.e. when IsRunningOnChromeOS() returns false. ##

--fake-drivefs-launcher-socket-path ##	A relative path to socket to communicat with the fake DriveFS launcher within the chroot specified by kFakeDriveFsLauncherChrootPath. This is only used when running on Linux, i.e. when IsRunningOnChromeOS() returns false. ##

--fake-oobe-configuration-file ##	Path to a OOBE configuration JSON file (used by FakeOobeConfigurationClient). ##

--fake-variations-channel ##	Fakes the channel of the browser for purposes of Variations filtering. This is to be used for testing only. Possible values are "stable", "beta", "dev" and "canary". This works for official builds as well. ##

--false ##	Value indicating whether flag from command line switch is false. ##

--feedback-server ##	Alternative feedback server to use when submitting user feedback ##

--field-trial-handle ##	Handle to the shared memory segment containing field trial state that is to be shared between processes. The argument to this switch is the handle id (pointer on Windows) as a string, followed by a comma, then the size of the shared memory segment as a string. ##

--file-url-path-alias ##	Define an alias root directory which is replaced with the replacement string in file URLs. The format is "/alias=/replacement", which would turn <file:///alias/some/path.html> into <file:///replacement/some/path.html>. ##

--file\_chooser ##	No description ##

--finch-seed-expiration-age ##	The length of time in seconds that an app's copy of the variations seed should be considered fresh. If an app's seed is older than this, a new seed will be requested from WebView's IVariationsSeedServer. ##

--finch-seed-ignore-pending-download ##	Forces WebView's service to always schedule a new variations seed download job, even if one is already pending. ##

--finch-seed-min-download-period ##	The minimum amount of time in seconds that WebView's service will wait between two variations seed downloads from the variations server. ##

--finch-seed-min-update-period ##	The minimum amount of time in seconds that the embedded WebView implementation will wait between two requests to WebView's service for a new variations seed. ##

--fingerprint-sensor-location ##	Fingerprint sensor location indicates the physical sensor's location. The value is a string with possible values: "power-button-top-left", "keyboard-bottom-left", keyboard-bottom-right", "keyboard-top-right". ##

--first-exec-after-boot ##	Passed to Chrome the first time that it's run after the system boots. Not passed on restart after sign out. ##

--flag-switches-begin ##	These two flags are added around the switches about:flags adds to the command line. This is useful to see which switches were added by about:flags on about:version. They don't have any effect. ##

--flag-switches-end ##	No description ##

--font-cache-shared-handle[1] ##	DirectWrite FontCache is shared by browser to renderers using shared memory. This switch allows us to pass the shared memory handle to the renderer. ##

--font-render-hinting ##	Sets font render hinting when running headless, affects Skia rendering and whether glyph subpixel positioning is enabled. Possible values: none|slight|medium|full|max. Default: full. ##

--force-app-mode ##	Forces application mode. This hides certain system UI elements and forces the app to be installed if it hasn't been already. ##

--force-caption-style ##	Forces the caption style for WebVTT captions. ##

--force-cert-verifier-builtin ##	Forces Chrome to use CertVerifyProcBuiltin for verification of server certificates, ignoring the status of net::features::kCertVerifierBuiltinFeature. ##

--force-color-profile ##	Force all monitors to be treated as though they have the specified color profile. Accepted values are "srgb" and "generic-rgb" (currently used by Mac layout tests) and "color-spin-gamma24" (used by layout tests). ##

--force-dark-mode ##	Forces dark mode in UI for platforms that support it. ##

--force-dev-mode-highlighting ##	Whether to force developer mode extensions highlighting. ##

--force-device-scale-factor ##	Overrides the device scale factor for the browser UI and the contents. ##

--force-devtools-available ##	Forces developer tools availability, no matter what values the enterprise policies DeveloperToolsDisabled and DeveloperToolsAvailability are set to. ##

--force-disable-variation-ids ##	Forces to remove Chrome Variation Ids from being sent in X-Client-Data header, specified as a 64-bit encoded list of numeric experiment ids. Ids prefixed with the character "t" will be treated as Trigger Variation Ids. ##

--force-effective-connection-type ##	Forces Network Quality Estimator (NQE) to return a specific effective connection type. ##

--force-empty-corb-allowlist ##	Whether |extensions\_features::kBypassCorbAllowlistParamName| should always be empty (i.e. ignoring hardcoded allowlist and the field trial param). This switch is useful for manually verifying if an extension would continue to work fine after removing it from the allowlist. ##

--force-enable-lite-pages ##	Force enable all available previews on every page load. ##

--force-enable-metrics-reporting ##	Forces metrics reporting to be enabled. ##

--force-enable-night-mode[7] ##	Forces the night mode to be enabled. ##

--force-enable-stylus-tools ##	Enables the stylus tools next to the status area. ##

--force-fieldtrial-params ##	This option can be used to force parameters of field trials when testing changes locally. The argument is a param list of (key, value) pairs prefixed by an associated (trial, group) pair. You specify the param list for multiple (trial, group) pairs with a comma separator. Example: "Trial1.Group1:k1/v1/k2/v2,Trial2.Group2:k3/v3/k4/v4" Trial names, groups names, parameter names, and value should all be URL escaped for all non-alphanumeric characters. ##

--force-fieldtrials ##	This option can be used to force field trials when testing changes locally. The argument is a list of name and value pairs, separated by slashes. If a trial name is prefixed with an asterisk, that trial will start activated. For example, the following argument defines two trials, with the second one activated: "GoogleNow/Enable/\*MaterialDesignNTP/Default/" This option can also be used by the browser process to send the list of trials to a non-browser process, using the same format. See FieldTrialList::CreateTrialsFromString() in field\_trial.h for details. ##

--force-first-run ##	Displays the First Run experience when the browser is started, regardless of whether or not it's actually the First Run (this overrides kNoFirstRun). ##

--force-first-run-ui ##	Forces first-run UI to be shown for every login. ##

--force-gpu-mem-available-mb ##	Sets the total amount of memory that may be allocated for GPU resources ##

--force-gpu-mem-discardable-limit-mb ##	Sets the maximum GPU memory to use for discardable caches. ##

--force-happiness-tracking-system ##	Force enables the Happiness Tracking System for the device. This ignores user profile check and time limits and shows the notification every time for any type of user. Should be used only for testing. ##

--force-high-contrast ##	Forces high-contrast mode in native UI drawing, regardless of system settings. Note that this has limited effect on Windows: only Aura colors will be switched to high contrast, not other system colors. ##

--force-hwid-check-failure-for-test ##	Forces Hardware ID check (happens during OOBE) to fail. Should be used only for testing. ##

--force-legacy-default-referrer-policy ##	Pins the default referrer policy to the pre-M80 value of no-referrer-when-downgrade. TODO(crbug.com/1016541): After M82, remove when the corresponding enterprise policy has been deleted. ##

--force-login-manager-in-tests ##	Usually in browser tests the usual login manager bringup is skipped so that tests can change how it's brought up. This flag disables that. ##

--force-logs-upload-failure ##	Force a logs upload failure to help test the logs upload retry. ##

--force-max-texture-size ##	Sets the maximum texture size in pixels. ##

--force-media-resolution-height ##	When present overrides the screen resolution used by CanDisplayType API, instead of using the values obtained from avsettings. ##

--force-media-resolution-width ##	No description ##

--force-overlay-fullscreen-video ##	Forces use of hardware overlay for fullscreen video playback. Useful for testing the Android overlay fullscreen functionality on other platforms. ##

--force-pnacl-subzero ##	Force use of the Subzero as the PNaCl translator instead of LLC. ##

--force-prefers-reduced-motion ##	Forces whether the user desires reduced motion, regardless of system settings. ##

--force-presentation-receiver-for-testing ##	This forces pages to be loaded as presentation receivers. Useful for testing behavior specific to presentation receivers. Spec: <https://www.w3.org/TR/presentation-api/#interface-presentationreceiver> ##

--force-protected-video-output-buffers[16] ##	Forces protected memory for all output video buffers generated by FuchsiaVideoDecoder, including unencrypted streams. Ignored unless --enable-protected-video-buffers is also specified. ##

--force-raster-color-profile ##	Force rastering to take place in the specified color profile. Accepted values are the same as for the kForceDisplayColorProfile case above. ##

--force-recovery-component ##	Force the recovery component, when running an unofficial build. This only applies to the cleaner, not the reporter. ##

--force-renderer-accessibility ##	Force renderer accessibility to be on instead of enabling it on demand when a screen reader is detected. The disable-renderer-accessibility switch overrides this if present. ##

--force-self-delete ##	Force self-deletion even on non-official builds. ##

--force-show-update-menu-badge[7] ##	Forces the update menu badge to show. ##

--force-stacked-tab-strip-layout ##	Forces Chrome to use a stacked tab strip layout. ##

--force-status-area-collapsible ##	Forces the status area to allow collapse/expand regardless of the current state. ##

--force-system-compositor-mode ##	Force system compositor mode when set. ##

--force-tablet-mode ##	Enables required things for the selected UI mode, regardless of whether the Chromebook is currently in the selected UI mode. ##

--force-tablet-power-button ##	If set, tablet-like power button behavior (i.e. tapping the button turns the screen off) is used even if the device is in laptop mode. ##

--force-text-direction ##	Force the text rendering to a specific direction. Valid values are "ltr" (left-to-right) and "rtl" (right-to-left). Only tested meaningfully with RTL. ##

--force-ui-direction ##	Force the UI to a specific direction. Valid values are "ltr" (left-to-right) and "rtl" (right-to-left). ##

--force-update-menu-type[7] ##	Forces the update menu type to a specific type. ##

--force-update-remote-url ##	Per-product customization of force update UI remote url, also used in testing. ##

--force-variation-ids ##	Forces additional Chrome Variation Ids that will be sent in X-Client-Data header, specified as a 64-bit encoded list of numeric experiment ids. Ids prefixed with the character "t" will be treated as Trigger Variation Ids. ##

--force-video-overlays ##	Force media player using SurfaceView instead of SurfaceTexture on Android. ##

--force-wave-audio[1] ##	Use Windows WaveOut/In audio API even if Core Audio is supported. ##

--force-webrtc-ip-handling-policy ##	Override WebRTC IP handling policy to mimic the behavior when WebRTC IP handling policy is specified in Preferences. ##

--force-webxr-runtime ##	Forcibly enable and select the specified runtime for webxr. Note that this provides an alternative means of enabling a runtime, and will also functionally disable all other runtimes. ##

--frame-throttle-fps ##	Sets the throttle fps for compositor frame submission. ##

--full-memory-crash-report ##	Generates full memory crash dump. ##

--gaia-config ##	No description ##

--gaia-url ##	No description ##

--gamepad-polling-interval ##	Overrides the gamepad polling interval. Decreasing the interval improves input latency of buttons and axes but may negatively affect performance due to more CPU time spent in the input polling thread. ##

--gcm-checkin-url ##	Sets the checkin service endpoint that will be used for performing Google Cloud Messaging checkins. ##

--gcm-mcs-endpoint ##	Sets the Mobile Connection Server endpoint that will be used for Google Cloud Messaging. ##

--gcm-registration-url ##	Sets the registration endpoint that will be used for creating new Google Cloud Messaging registrations. ##

--generate-accessibility-test-expectations ##	For development / testing only. When running content\_browsertests, saves output of failing accessibility tests to their expectations files in content/test/data/accessibility/, overwriting existing file content. ##

--gl ##	No description ##

--gl-egl ##	No description ##

--gl-null ##	No description ##

--gl-shader-interm-output ##	Include ANGLE's intermediate representation (AST) output in shader compilation info logs. ##

--gles ##	No description ##

--gles-egl ##	No description ##

--gles-null ##	No description ##

--google-apis-url ##	No description ##

--google-base-url ##	Specifies an alternate URL to use for speaking to Google. Useful for testing. ##

--google-doodle-url ##	Overrides the URL used to fetch the current Google Doodle. Example: <https://www.google.com/async/ddljson> Testing? Try: <https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_android0.json> <https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_android1.json> <https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_android2.json> <https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_android3.json> <https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_android4.json> ##

--google-url ##	No description ##

--gpu ##	No description ##

--gpu-blocklist-test-group ##	Select a different set of GPU blocklist entries with the specified test\_group ID. ##

--gpu-device-id ##	Passes the active graphics device id from browser process to info collection GPU process. ##

--gpu-driver-bug-list-test-group ##	Enable an extra set of GPU driver bug list entries with the specified test\_group ID. Note the default test group (group 0) is still active. ##

--gpu-driver-version ##	Passes the active graphics driver version from browser process to info collection GPU process. ##

--gpu-launcher ##	Extra command line options for launching the GPU process (normally used for debugging). Use like renderer-cmd-prefix. ##

--gpu-no-context-lost ##	Inform Chrome that a GPU context will not be lost in power saving mode, screen saving mode, etc. Note that this flag does not ensure that a GPU context will never be lost in any situations, say, a GPU reset. ##

--gpu-preferences ##	Passes encoded GpuPreferences to GPU process. ##

--gpu-process ##	Flags spied upon from other layers. ##

--gpu-program-cache-size-kb ##	Sets the maximum size of the in-memory gpu program cache, in kb ##

--gpu-rasterization-msaa-sample-count ##	The number of multisample antialiasing samples for GPU rasterization. Requires MSAA support on GPU to have an effect. 0 disables MSAA. ##

--gpu-revision ##	Passes the active graphics revision info from browser process to info collection GPU process. ##

--gpu-sandbox-allow-sysv-shm ##	Allows shmat() system call in the GPU sandbox. ##

--gpu-sandbox-failures-fatal ##	Makes GPU sandbox failures fatal. ##

--gpu-sandbox-start-early ##	Starts the GPU sandbox before creating a GL context. ##

--gpu-startup-dialog ##	Causes the GPU process to display a dialog on launch. ##

--gpu-sub-system-id ##	Passes the active graphics sub system id from browser process to info collection GPU process. ##

--gpu-vendor-id ##	Passes the active graphics vendor id from browser process to info collection GPU process. ##

--gpu2-startup-dialog[1] ##	Causes the second GPU process used for gpu info collection to display a dialog on launch. ##

--graphics-buffer-count ##	No description ##

--guest[15] ##	Causes the browser to launch directly in guest mode. ##

--guest-wallpaper-large ##	Large wallpaper to use in guest mode (as path to trusted, non-user-writable JPEG file). ##

--guest-wallpaper-small ##	Small wallpaper to use in guest mode (as path to trusted, non-user-writable JPEG file). ##

--h[9] ##	No description ##

--has-chromeos-keyboard ##	If set, the system is a Chromebook with a "standard Chrome OS keyboard", which generally means one with a Search key in the standard Caps Lock location above the Left Shift key. It should be unset for Chromebooks with both Search and Caps Lock keys (e.g. stout) and for devices like Chromeboxes that only use external keyboards. ##

--has-internal-stylus ##	Whether this device has an internal stylus. ##

--headless ##	Run in headless mode, i.e., without a UI or display server dependencies. ##

--help ##	No description ##

--hermes-fake ##	Enables Hermes fake behavior. By default, no carrier profiles are setup. If a value of "on" is passed for this switch, then hermes fakes are initialized with a single installed carrier profile. Fake cellular service corresponding to carrier profiles are also setup in Shill. ##

--hide ##	"Hide" value for kCrosRegionsMode (VPD values are hidden). ##

--hide-android-files-in-files-app ##	If true, files in Android internal storage will be hidden in Files app. ##

--hide-icons[1] ##	Makes Windows happy by allowing it to show "Enable access to this program" checkbox in Add/Remove Programs->Set Program Access and Defaults. This only shows an error box because the only way to hide Chrome is by uninstalling it. ##

--hide-scrollbars ##	Hide scrollbars from screenshots. ##

--highlight-all-webviews ##	Highlight the contents (including web contents) of all WebViews with a yellow tint. This is useful for identifying WebViews in an Android application. ##

--highlight-non-lcd-text-layers ##	Highlights layers that can't use lcd text. Layers containing no text won't be highlighted. See DebugColors::NonLCDTextHighlightColor() for the colors. ##

--homedir ##	Defines user homedir. This defaults to primary user homedir. ##

--homepage ##	Specifies which page will be displayed in newly-opened tabs. We need this for testing purposes so that the UI tests don't depend on what comes up for <http://google.com>. ##

--host ##	No description ##

--host-resolver-rules ##	These mappings only apply to the host resolver. ##

--<https://chromemediarecommendations-pa.googleapis.com> ##	No description ##

--icon\_reader[1] ##	No description ##

--ignore-autocomplete-off-autofill ##	Ignores autocomplete="off" for Autofill data (profiles + credit cards). ##

--ignore-certificate-errors-spki-list ##	A set of public key hashes for which to ignore certificate-related errors. If the certificate chain presented by the server does not validate, and one or more certificates have public key hashes that match a key from this list, the error is ignored. The switch value must a be a comma-separated list of Base64-encoded SHA-256 SPKI Fingerprints (RFC 7469, Section 2.4). This switch has no effect unless --user-data-dir (as defined by the content embedder) is also present. ##

--ignore-google-port-numbers ##	When set, this will ignore the PortPermission passed in the google\_util.h methods and ignore the port numbers. This makes it easier to run tests for features that use these methods (directly or indirectly) with the EmbeddedTestServer, which is more representative of production. ##

--ignore-gpu-blacklist ##	Ignores GPU blocklist. TODO(crbug.com/1101491): remove in 2020Q4 in favor of --ignore-gpu-blocklist. ##

--ignore-gpu-blocklist ##	Ignores GPU blocklist. ##

--ignore-previews-blacklist ##	Ignore decisions made by PreviewsBlockList. TODO(crbug.com/1092105) : Migrate this to ignore-previews-blacklist. ##

--ignore-urlfetcher-cert-requests ##	Causes net::URLFetchers to ignore requests for SSL client certificates, causing them to attempt an unauthenticated SSL/TLS session. This is intended for use when testing various service URLs (eg: kPromoServerURL, kSbURLPrefix, kSyncServiceURL, etc). ##

--ignore-user-profile-mapping-for-tests ##	If true, profile selection in UserManager will always return active user's profile. TODO(nkostlyev): <http://crbug.com/364604> - Get rid of this switch after we turn on multi-profile feature on ChromeOS. ##

--ime[5] ##	No description ##

--in-process-gpu ##	Run the GPU process as a thread in the browser process. ##

--incognito ##	No description ##

--init-done-notifier ##	The handle of an event to signal when the initialization of the main process is complete (including loading all DLL's). This is used by the integration test to check that forbidden modules aren't loaded outside the sandbox. If this is set, the main process will signal the event and then wait for the integration test to signal it as well before continuing. Ignored in official builds. ##

--initial-virtual-time ##	Start the renderer with an initial virtual time override specified in seconds since the epoch. ##

--input ##	No description ##

--install-autogenerated-theme ##	Installs an autogenerated theme based on the given RGB value. The format is "r,g,b", where r, g, b are a numeric values from 0 to 255. ##

--install-chrome-app ##	Causes Chrome to initiate an installation flow for the given app. ##

--install-managed-bookmarks-handler ##	Installs the managed bookmarks policy handler. ##

--install-supervised-user-whitelists ##	A list of whitelists to install for a supervised user, for testing. The list is of the following form: <id>[:<name>],[<id>[:<name>],...] ##

--install-url-blocklist-handlers ##	Installs the URLBlocklist and URLAllowlist handlers. ##

--instant-process ##	Marks a renderer as an Instant process. ##

--integration-test-timeout-minutes ##	Set the timeout for integration tests in minutes. 0 disables the timeout entirely. ##

--ipc-connection-timeout ##	Overrides the timeout, in seconds, that a child process waits for a connection from the browser before killing itself. ##

--ipc-dump-directory[18] ##	Dumps IPC messages sent from renderer processes to the browser process to the given directory. Used primarily to gather samples for IPC fuzzing. ##

--ipc-fuzzer-testcase[18] ##	Specifies the testcase used by the IPC fuzzer. ##

--isolate-origins ##	Require dedicated processes for a set of origins, specified as a comma-separated list. For example: --isolate-origins=https://www.foo.com,https://www.bar.com ##

--javascript-harmony ##	Enables experimental Harmony (ECMAScript 6) features. ##

--js-flags ##	Specifies the flags passed to JS engine. ##

--kaleidoscope-backend-url ##	A command line switch for overriding the base URL of the API. ##

--keep-alive-for-test ##	Used for testing - keeps browser alive after last browser window closes. ##

--kernelnext-restrict-vms ##	If set, the Chrome settings will not expose the option to enable crostini unless the enable-experimental-kernel-vm-support flag is set in chrome://flags ##

--kiosk ##	Enable kiosk mode. Please note this is not Chrome OS kiosk mode. ##

--kiosk-printing ##	Enable automatically pressing the print button in print preview. ##

--lacros-chrome-additional-args ##	If this switch is set, then ash-chrome will pass additional arguments when launching lacros-chrome. The string '####' is used as a delimiter. Example: --lacros-chrome-additional-args="--foo=5####--bar=/tmp/dir name". Will result in two arguments passed to lacros-chrome: --foo=5 --bar=/tmp/dir name ##

--lacros-chrome-path ##	If this switch is set, then ash-chrome will exec the lacros-chrome binary from the indicated path rather than from component updater. Note that the path should be to a directory that contains a binary named 'chrome'. ##

--lang ##	The language file that we want to try to open. Of the form language[-country] where language is the 2 letter code from ISO-639. ##

--last-launched-app ##	Pass the app id information to the renderer process, to be used for logging. last-launched-app should be the app that just launched and is spawning the renderer. ##

--layer ##	No description ##

--light ##	No description ##

--lite-video-default-downlink-bandwidth-kbps ##	The default downlink bandwidth estimate used for throttling media requests. Only used when forcing LiteVideos to be allowed. ##

--lite-video-force-coinflip-holdback ##	Forces the coinflip used for a counterfactual experiment to be true. ##

--lite-video-force-override-decision ##	Overrides all the LiteVideo decision logic to allow it on every navigation. This causes LiteVideos to ignore the hints, user blocklist, and network condition. ##

--lite-video-ignore-network-conditions ##	Overrides the network conditions checks for LiteVideos. ##

--load-and-launch-app ##	Loads an app from the specified directory and launches it. ##

--load-apps ##	Comma-separated list of paths to apps to load at startup. The first app in the list will be launched. ##

--load-empty-dll ##	Attempt to load empty-dll.dll whenever this flag is set. For testing DLL loading. ##

--load-extension ##	Comma-separated list of paths to extensions to load at startup. ##

--load-media-router-component-extension ##	Loads the Media Router component extension on startup. ##

--load-signin-profile-test-extension[5] ##	Path to the unpacked test extension to load into the signin profile. The ID extension loaded must match kTestSigninProfileExtensionId. ##

--local-sync-backend-dir ##	Specifies the local sync backend directory. The name is chosen to mimic user-data-dir etc. This flag only matters if the enable-local-sync-backend flag is present. ##

--log-best-effort-tasks ##	Logs information about all tasks posted with TaskPriority::BEST\_EFFORT. Use this to diagnose issues that are thought to be caused by TaskPriority::BEST\_EFFORT execution fences. Note: Tasks posted to a non-BEST\_EFFORT UpdateableSequencedTaskRunner whose priority is later lowered to BEST\_EFFORT are not logged. ##

--log-file ##	Overrides the default file name to use for general-purpose logging (does not affect which events are logged). ##

--log-gpu-control-list-decisions ##	Logs GPU control list decisions when enforcing blacklist rules. ##

--log-interface-calls-to ##	Specifies a file to log all the interface calls of EngineRequestsImpl and CleanerEngineRequestsImpl. ##

--log-level ##	Sets the minimum log level. Valid values are from 0 to 3: INFO = 0, WARNING = 1, LOG\_ERROR = 2, LOG\_FATAL = 3. ##

--log-net-log ##	Enables saving net log events to a file. If a value is given, it used as the path the the file, otherwise the file is named netlog.json and placed in the user data directory. ##

--log-on-ui-double-background-blur[19] ##	Checks and logs double background blur as an error if any. ##

--log-with-hash ##	Log user actions with action name hashed. ##

--log-without-hash ##	Log user actions with action name unhashed. ##

--login-manager ##	Enables Chrome-as-a-login-manager behavior. ##

--login-profile ##	Specifies the profile to use once a chromeos user is logged in. This parameter is ignored if user goes through login screen since user\_id hash defines which profile directory to use. In case of browser restart within active session this parameter is used to pass user\_id hash for primary user. ##

--login-user ##	Specifies the user which is already logged in. ##

--logs-upload-retry-interval ##	Specify the time to wait between logs upload retries, in minutes. ##

--lso-url ##	No description ##

--ltr ##	No description ##

--main-frame-resizes-are-orientation-changes ##	Resizes of the main frame are caused by changing between landscape and portrait mode (i.e. Android) so the page should be rescaled to fit. ##

--make-chrome-default[6] ##	Indicates whether Chrome should be set as the default browser during installation. ##

--make-default-browser ##	Makes Chrome default browser ##

--managed-user-id ##	Sets the supervised user ID for any loaded or newly created profile to the given value. Pass an empty string to mark the profile as non-supervised. Used for testing. ##

--mangle-localized-strings ##	Transform localized strings to be longer, with beginning and end markers to make truncation visually apparent. ##

--manual ##	No description ##

--market-url-for-testing[7] ##	Sets the market URL for Chrome for use in testing. ##

--marketing-opt-in-url ##	Determines the URL to be used when calling the backend. ##

--max-active-webgl-contexts ##	Allows user to override maximum number of active WebGL contexts per renderer process. ##

--max-decoded-image-size-mb ##	Sets the maximum decoded image size limitation. ##

--max-file-size ##	Limit the size of files the scanning engine is allowed to open. ##

--max-gum-fps ##	Override the maximum framerate as can be specified in calls to getUserMedia. This flag expects a value. Example: --max-gum-fps=17.5 ##

--max-output-volume-dba1m ##	Calibrated max output volume dBa for voice content at 1 meter, if known. ##

--max-untiled-layer-height ##	Sets the width and height above which a composited layer will get tiled. ##

--max-untiled-layer-width ##	No description ##

--mem-pressure-system-reserved-kb ##	Some platforms typically have very little 'free' memory, but plenty is available in buffers+cached. For such platforms, configure this amount as the portion of buffers+cached memory that should be treated as unavailable. If this switch is not used, a simple pressure heuristic based purely on free memory will be used. ##

--memlog ##	No description ##

--memlog-sampling-rate ##	No description ##

--memlog-stack-mode ##	No description ##

--memory-pressure-off ##	No description ##

--message-loop-type-ui ##	Indicates the utility process should run with a message loop type of UI. ##

--metrics-client-id[6] ##	This is how the metrics client ID is passed from the browser process to its children. With Crashpad, the metrics client ID is distinct from the crash client ID. ##

--metrics-recording-only ##	Enables the recording of metrics reports but disables reporting. In contrast to kForceEnableMetricsReporting, this executes all the code that a normal client would use for reporting, except the report is dropped rather than sent to the server. This is useful for finding issues in the metrics code during UI and performance tests. ##

--metrics-upload-interval ##	Override the standard time interval between each metrics report upload for UMA and UKM. It is useful to set to a short interval for debugging. Unit in seconds. (The default is 1800 seconds on desktop). ##

--min-height-for-gpu-raster-tile ##	Sets the min tile height for GPU raster. ##

--minimal ##	No description ##

--mixed ##	No description ##

--mixer-enable-dynamic-channel-count ##	Enable dynamically changing the channel count in the mixer depending on the input streams. ##

--mixer-service-endpoint ##	Endpoint that the mixer service listens on. This is a path for a UNIX domain socket (default is /tmp/mixer-service). ##

--mixer-service-port ##	TCP port that the mixer service listens on on non-Linux platforms. (default 12854). ##

--mixer-source-audio-ready-threshold-ms ##	Specify the start threshold frames for audio output when using our mixer. This is mostly used to override the default value to a larger value, for platforms that can't handle the default start threshold without running into audio underruns. ##

--mixer-source-input-queue-ms ##	Specify the buffer size for audio output when using our mixer. This is mostly used to override the default value to a larger value, for platforms that can't handle an audio buffer so small without running into audio underruns. ##

--mock ##	No description ##

--mock-cert-verifier-default-result-for-testing ##	Set the default result for MockCertVerifier. This only works in test code. ##

--mojo-core-library-path ##	Initializes Mojo Core from a shared library at the specified path, rather than using the version of Mojo Core embedded within the Content executable. ##

--mojo-local-storage ##	Use a Mojo-based LocalStorage implementation. ##

--mojo-pipe-token ##	No description ##

--monitoring-destination-id ##	Allows setting a different destination ID for connection-monitoring GCM messages. Useful when running against a non-prod management server. ##

--mse-audio-buffer-size-limit-mb ##	Allows explicitly specifying MSE audio/video buffer sizes as megabytes. Default values are 150M for video and 12M for audio. ##

--mse-video-buffer-size-limit-mb ##	No description ##

--mute-audio ##	Mutes audio sent to the audio device so it is not audible during automated testing. ##

--nacl-broker ##	No description ##

--nacl-dangerous-no-sandbox-nonsfi ##	Disable sandbox even for non SFI mode. This is particularly unsafe as non SFI NaCl heavily relies on the seccomp sandbox. ##

--nacl-debug-mask ##	Uses NaCl manifest URL to choose whether NaCl program will be debugged by debug stub. Switch value format: [!]pattern1,pattern2,...,patternN. Each pattern uses the same syntax as patterns in Chrome extension manifest. The only difference is that \* scheme matches all schemes instead of matching only http and https. If the value doesn't start with !, a program will be debugged if manifest URL matches any pattern. If the value starts with !, a program will be debugged if manifest URL does not match any pattern. ##

--nacl-gdb ##	Native Client GDB debugger that will be launched automatically when needed. ##

--nacl-gdb-script ##	GDB script to pass to the nacl-gdb debugger at startup. ##

--nacl-loader ##	No description ##

--nacl-loader-nonsfi ##	Value for --type that causes the process to run as a NativeClient loader for non SFI mode. ##

--native ##	No description ##

--native-messaging-connect-extension ##	Requests a native messaging connection be established between the extension with ID specified by this switch and the native messaging host named by the kNativeMessagingConnectHost switch. ##

--native-messaging-connect-host ##	Requests a native messaging connection be established between the native messaging host named by this switch and the extension with ID specified by kNativeMessagingConnectExtension. ##

--native-messaging-connect-id ##	If set when kNativeMessagingConnectHost and kNativeMessagingConnectExtension are specified, is reflected to the native messaging host as a command line parameter. ##

--native-with-thread-names ##	No description ##

--nearbysharing-http-host ##	Overrides the default URL for Google APIs (<https://www.googleapis.com>) used by Nearby Share ##

--need-arc-migration-policy-check ##	If present, the device needs to check the policy to see if the migration to ext4 for ARC is allowed. It should be present only on devices that have been initially issued with ecrypfs encryption and have ARC (N+) available. For the devices in other categories this flag must be missing. ##

--net-log-capture-mode ##	Sets the granularity of events to capture in the network log. The mode can be set to one of the following values: "Default" "IncludeSensitive" "Everything" See the enums of the corresponding name in net\_log\_capture\_mode.h for a description of their meanings. ##

--netifs-to-ignore ##	List of network interfaces to ignore. Ignored interfaces will not be used for network connectivity. ##

--network ##	No description ##

--network-country-iso[7] ##	The telephony region (ISO country code) to use in phone number detection. ##

--network-quiet-timeout ##	Sets the timeout seconds of the network-quiet timers in IdlenessDetector. Used by embedders who want to change the timeout time in order to run web contents on various embedded devices and changeable network bandwidths in different regions. For example, it's useful when using FirstMeaningfulPaint signal to dismiss a splash screen. ##

--new-canvas-2d-api ##	Enable in-progress canvas 2d API features. ##

--new-window ##	Launches URL in new browser window. ##

--no-crash-upload ##	Prevent the crash client from uploading crash reports. ##

--no-default-browser-check ##	Disables the default browser check. Useful for UI/browser tests where we want to avoid having the default browser info-bar displayed. ##

--no-delay-for-dx12-vulkan-info-collection ##	Start the non-sandboxed GPU process for DX12 and Vulkan info collection immediately after the browser starts. The default is to delay for 120 seconds. ##

--no-experiments ##	Disables all experiments set on about:flags. Does not disable about:flags itself. Useful if an experiment makes chrome crash at startup: One can start chrome with --no-experiments, disable the problematic lab at about:flags and then restart chrome without this switch again. ##

--no-first-run ##	Skip First Run tasks, whether or not it's actually the First Run. Overridden by kForceFirstRun. This does not drop the First Run sentinel and thus doesn't prevent first run from occurring the next time chrome is launched without this flag. ##

--no-initial-navigation ##	Stops new Shell objects from navigating to a default url. ##

--no-managed-user-acknowledgment-check ##	Disables checking whether we received an acknowledgment when registering a supervised user. Also disables the timeout during registration that waits for the ack. Useful when debugging against a server that does not support notifications. ##

--no-network-profile-warning[1] ##	Whether or not the browser should warn if the profile is on a network share. This flag is only relevant for Windows currently. ##

--no-pings ##	Don't send hyperlink auditing pings ##

--no-proxy-server ##	Don't use a proxy server, always make direct connections. Overrides any other proxy server flags that are passed. ##

--no-recovery-component ##	Prevent downloading and running the recovery component. ##

--no-report-upload ##	Prevent the logging service from uploading logs and reports. WARNING: this switch is used by internal test systems. Be careful when making changes. ##

--no-sandbox ##	Disables the sandbox for all process types that are normally sandboxed. Meant to be used as a browser-level switch for testing purposes only. ##

--no-sandbox-and-elevated[1] ##	Disables the sandbox and gives the process elevated privileges. ##

--no-self-delete ##	Prevent the executable from deleting itself after running. ##

--no-service-autorun ##	Disables the service process from adding itself as an autorun process. This does not delete existing autorun registrations, it just prevents the service from registering a new one. ##

--no-startup-window ##	Does not automatically open a browser window on startup (used when launching Chrome for the purpose of hosting background apps). ##

--no-user-gesture-required ##	Autoplay policy that does not require any user gesture. ##

--no-v8-untrusted-code-mitigations ##	Disables V8 mitigations for executing untrusted code. ##

--no-vr-runtime ##	Tell WebXr to assume that it does not support any runtimes. ##

--no-wifi ##	Disable features that require WiFi management. ##

--no-xshm ##	Disables MIT-SHM extension. ##

--no-zygote ##	Disables the use of a zygote process for forking child processes. Instead, child processes will be forked and exec'd directly. Note that --no-sandbox should also be used together with this flag because the sandbox needs the zygote to work. ##

--no-zygote-sandbox[10] ##	Instructs the zygote to launch without a sandbox. Processes forked from this type of zygote will apply their own custom sandboxes later. ##

--noerrdialogs ##	Suppresses all error dialogs when present. ##

--none ##	Must be in sync with "sandbox\_type" values as used in service manager's manifest.json catalog files. ##

--none\_and\_elevated ##	No description ##

--note-taking-app-ids ##	An optional comma-separated list of IDs of apps that can be used to take notes. If unset, a hardcoded list is used instead. ##

--notification-inline-reply[1] ##	Used in combination with kNotificationLaunchId to specify the inline reply entered in the toast in the Windows Action Center. ##

--notification-launch-id[1] ##	Used for launching Chrome when a toast displayed in the Windows Action Center has been activated. Should contain the launch ID encoded by Chrome. ##

--ntp-snippets-add-incomplete ##	If this flag is set, we will add downloaded snippets that are missing some critical data to the list. ##

--null ##	No description ##

--num-raster-threads ##	Number of worker threads used to rasterize content. ##

--oauth-account-manager-url ##	No description ##

--oauth2-client-id ##	No description ##

--oauth2-client-secret ##	No description ##

--oculus ##	The following are the runtimes that WebXr supports. ##

--offer-in-settings ##	No description ##

--on-the-fly-mhtml-hash-computation ##	Calculate the hash of an MHTML file as it is being saved. The browser process will write the serialized MHTML contents to a file and calculate its hash as it is streamed back from the renderer via a Mojo data pipe. ##

--oobe-eula-url-for-tests ##	Allows the eula url to be overridden for tests. ##

--oobe-force-show-screen ##	Forces OOBE/login to force show a comma-separated list of screens from chromeos::kScreenNames in oobe\_screen.cc. Supported screens are: user-image ##

--oobe-force-tablet-first-run ##	Indicates that the first user run flow (sequence of OOBE screens after the first user login) should show tablet mode centric screens, even if the device is not in tablet mode. ##

--oobe-guest-session ##	Indicates that a guest session has been started before OOBE completion. ##

--oobe-screenshot-dir ##	Specifies directory for screenshots taken with OOBE UI Debugger. ##

--oobe-skip-postlogin ##	Skips all other OOBE pages after user login. ##

--oobe-skip-to-login ##	Skip to login screen. ##

--oobe-timer-interval ##	Interval at which we check for total time on OOBE. ##

--oobe-timezone-override-for-tests ##	Allows the timezone to be overridden on the marketing opt-in screen. ##

--opengraph ##	No description ##

--openvr ##	No description ##

--openxr ##	No description ##

--optimization-guide-fetch-hints-override ##	Overrides scheduling and time delays for fetching hints and causes a hints fetch immediately on start up using the provided comma separate lists of hosts. ##

--optimization-guide-fetch-hints-override-timer ##	Overrides the hints fetch scheduling and delay, causing a hints fetch immediately on start up using the TopHostProvider. This is meant for testing. ##

--optimization-guide-fetch-models-and-features-override ##	Overrides scheduling and time delays for fetching prediction models and host model features. This causes a prediction model and host model features fetch immediately on start up. ##

--optimization-guide-service-api-key ##	Overrides the Optimization Guide Service API Key for remote requests to be made. ##

--optimization-guide-service-get-hints-url ##	Overrides the Optimization Guide Service URL that the HintsFetcher will request remote hints from. ##

--optimization-guide-service-get-models-url ##	Overrides the Optimization Guide Service URL that the PredictionModelFetcher will request remote models and host features from. ##

--optimization\_guide\_hints\_override ##	Overrides the Hints Protobuf that would come from the component updater. If the value of this switch is invalid, regular hint processing is used. The value of this switch should be a base64 encoding of a binary Configuration message, found in optimization\_guide's hints.proto. Providing a valid value to this switch causes Chrome startup to block on hints parsing. ##

--orientation-sensors ##	No description ##

--origin-trial-disabled-features ##	Contains a list of feature names for which origin trial experiments should be disabled. Names should be separated by "|" characters. ##

--origin-trial-disabled-tokens ##	Contains a list of token signatures for which origin trial experiments should be disabled. Tokens should be separated by "|" characters. ##

--origin-trial-public-key ##	Comma-separated list of keys which will override the default public keys for checking origin trial tokens. ##

--output ##	No description ##

--override ##	"Override" value for kCrosRegionsMode (region's data is read first). ##

--override-enabled-cdm-interface-version ##	Overrides the default enabled library CDM interface version(s) with the one specified with this switch, which will be the only version enabled. For example, on a build where CDM 8, CDM 9 and CDM 10 are all supported (implemented), but only CDM 8 and CDM 9 are enabled by default: --override-enabled-cdm-interface-version=8 : Only CDM 8 is enabled --override-enabled-cdm-interface-version=9 : Only CDM 9 is enabled --override-enabled-cdm-interface-version=10 : Only CDM 10 is enabled --override-enabled-cdm-interface-version=11 : No CDM interface is enabled This can be used for local testing and debugging. It can also be used to enable an experimental CDM interface (which is always disabled by default) for testing while it's still in development. ##

--override-hardware-secure-codecs-for-testing ##	Overrides hardware secure codecs support for testing. If specified, real platform hardware secure codecs check will be skipped. Codecs are separated by comma. Valid codecs are "vp8", "vp9" and "avc1". For example: --override-hardware-secure-codecs-for-testing=vp8,vp9 --override-hardware-secure-codecs-for-testing=avc1 CENC encryption scheme is assumed to be supported for the specified codecs. If no valid codecs specified, no hardware secure codecs are supported. This can be used to disable hardware secure codecs support: --override-hardware-secure-codecs-for-testing ##

--override-https-image-compression-infobar ##	Override the one-time InfoBar to not needed to be shown before triggering https image compression for the page load. ##

--override-metrics-upload-url ##	Override the URL to which metrics logs are sent for debugging. ##

--override-plugin-power-saver-for-testing ##	Override the behavior of plugin throttling for testing. By default the throttler is only enabled for a hard-coded list of plugins. Set the value to 'always' to always throttle every plugin instance. Set the value to 'never' to disable throttling. ##

--override-use-software-gl-for-tests ##	Forces the use of software GL instead of hardware gpu. ##

--ozone-dump-file ##	Specify location for image dumps. ##

--ozone-platform ##	Specify ozone platform implementation to use. ##

--pack-extension ##	Packages an extension to a .crx installable file from a given directory. ##

--pack-extension-key ##	Optional PEM private key to use in signing packaged .crx. ##

--parent-window ##	No description ##

--passive-listeners-default ##	Override the default value for the 'passive' field in javascript addEventListener calls. Values are defined as: 'documentonlytrue' to set the default be true only for document level nodes. 'true' to set the default to be true on all nodes (when not specified). 'forcealltrue' to force the value on all nodes. ##

--passthrough ##	No description ##

--password-store ##	Specifies which encryption storage backend to use. Possible values are kwallet, kwallet5, gnome, gnome-keyring, gnome-libsecret, basic. Any other value will lead to Chrome detecting the best backend automatically. TODO(crbug.com/571003): Once PasswordStore no longer uses the Keyring or KWallet for storing passwords, rename this flag to stop referencing passwords. Do not rename it sooner, though; developers and testers might rely on it keeping large amounts of testing passwords out of their Keyrings or KWallets. ##

--pdf\_conversion[1] ##	No description ##

--pen-devices[10] ##	Tells chrome to interpret events from these devices as pen events. Only available with XInput 2 (i.e. X server 1.8 or above). The id's of the devices can be retrieved from 'xinput list'. ##

--perf-test-print-uma-means ##	Show the mean value of histograms that native performance tests are monitoring. Note that this is only applicable for PerformanceTest subclasses. ##

--perfetto-disable-interning ##	Repeat internable data for each TraceEvent in the perfetto proto format. ##

--perfetto-output-file ##	If supplied, will enable Perfetto startup tracing and stream the output to the given file. On Android, if no file is provided, automatically generate a file to write the output to. TODO(oysteine): Remove once Perfetto starts early enough after process startup to be able to replace the legacy startup tracing. ##

--performance[5] ##	No description ##

--permission-request-api-scope ##	Development flag for permission request API. This flag is needed until the API is finalized. TODO(bauerb): Remove when this flag is not needed anymore. ##

--permission-request-api-url ##	Development flag for permission request API. This flag is needed until the API is finalized. TODO(bauerb): Remove when this flag is not needed anymore. ##

--playready-key-system ##	No description ##

--post-reboot ##	Indicates this run is post-reboot. ##

--post-reboot-override-cmd ##	Indicates that this is a post-reboot run that should fetch its switches from an external registry key. Important note: When this switch is present in the command line, all other switches are ignored except for kCleanupIdSwitch, which is used to read post-reboot switches from the correct registry location. ##

--post-reboot-trigger ##	A freeform text string that records what triggered a post-reboot run. ##

--ppapi ##	No description ##

--ppapi-antialiased-text-enabled[1] ##	The boolean value (0/1) of FontRenderParams::antialiasing to be passed to Ppapi processes. ##

--ppapi-broker ##	No description ##

--ppapi-flash-args ##	"Command-line" arguments for the PPAPI Flash; used for debugging options. ##

--ppapi-flash-path ##	Use the PPAPI (Pepper) Flash found at the given path. ##

--ppapi-flash-version ##	Report the given version for the PPAPI (Pepper) Flash. The version should be numbers separated by '.'s (e.g., "12.3.456.78"). If not specified, it defaults to "10.2.999.999". ##

--ppapi-in-process ##	Runs PPAPI (Pepper) plugins in-process. ##

--ppapi-plugin-launcher ##	Specifies a command that should be used to launch the ppapi plugin process. Useful for running the plugin process through purify or quantify. Ex: --ppapi-plugin-launcher="path\to\purify /Run=yes" ##

--ppapi-startup-dialog ##	Causes the PPAPI sub process to display a dialog on launch. Be sure to use --no-sandbox as well or the sandbox won't allow the dialog to display. ##

--ppapi-subpixel-rendering-setting[1] ##	The enum value of FontRenderParams::subpixel\_rendering to be passed to Ppapi processes. ##

--previous-app ##	previous-app should be the app that was running when last-launched-app started. ##

--print-to-pdf ##	Save a pdf file of the loaded page. ##

--print-to-pdf-no-header ##	Do not display header and footer in the pdf file. ##

--print\_compositor ##	No description ##

--privet-ipv6-only ##	Use IPv6 only for privet HTTP. ##

--process-per-site ##	Enable the "Process Per Site" process model for all domains. This mode consolidates same-site pages so that they share a single process. More details here: - <https://www.chromium.org/developers/design-documents/process-models> - The class comment in site\_instance.h, listing the supported process models. IMPORTANT: This isn't to be confused with --site-per-process (which is about isolation, not consolidation). You probably want the other one. ##

--process-per-tab ##	Runs each set of script-connected tabs (i.e., a BrowsingInstance) in its own renderer process. We default to using a renderer process for each site instance (i.e., group of pages from the same registered domain with script connections to each other). TODO(creis): This flag is currently a no-op. We should refactor it to avoid "unnecessary" process swaps for cross-site navigations but still swap when needed for security (e.g., isolated origins). ##

--prod ##	The command line alias and URL for the "prod" environment. ##

--product-version ##	Outputs the product version information and quit. Used as an internal api to detect the installed version of Chrome on Linux. ##

--profile-directory ##	Selects directory of profile to associate with the first browser launched. ##

--profile-requires-policy ##	If set to "true", the profile requires policy during restart (policy load must succeed, otherwise session restart should fail). ##

--profiling-at-start ##	Starts the sampling based profiler for the browser process at startup. This will only work if chrome has been built with the gn arg enable\_profiling = true. The output will go to the value of kProfilingFile. ##

--profiling-file ##	Specifies a location for profiling output. This will only work if chrome has been built with the gyp variable profiling=1 or gn arg enable\_profiling=true. {pid} if present will be replaced by the pid of the process. {count} if present will be incremented each time a profile is generated for this process. The default is chrome-profile-{pid} for the browser and test-profile-{pid} for tests. ##

--profiling-flush ##	Controls whether profile data is periodically flushed to a file. Normally the data gets written on exit but cases exist where chromium doesn't exit cleanly (especially when using single-process). A time in seconds can be specified. ##

--progress-bar-animation[7] ##	Specifies Android phone page loading progress bar animation. ##

--proxy-auto-detect ##	Forces proxy auto-detection. ##

--proxy-bypass-list ##	Specifies a list of hosts for whom we bypass proxy settings and use direct connections. Ignored unless --proxy-server is also specified. This is a comma-separated list of bypass rules. See: "net/proxy\_resolution/proxy\_bypass\_rules.h" for the format of these rules. ##

--proxy-pac-url ##	Uses the pac script at the given URL ##

--proxy-server ##	Uses a specified proxy server, overrides system settings. This switch only affects HTTP and HTTPS requests. ##

--proxy\_resolver[1] ##	No description ##

--pseudo ##	No description ##

--public-accounts-saml-acl-url ##	SAML assertion consumer URL, used to detect when Gaia-less SAML flows end (e.g. for SAML managed guest sessions) TODO(984021): Remove when URL is sent by DMServer. ##

--pull-to-refresh ##	Enables or disables pull-to-refresh gesture in response to vertical overscroll. Set the value to '0' to disable the feature, set to '1' to enable it for both touchpad and touchscreen, and set to '2' to enable it only for touchscreen. Defaults to disabled. ##

--purge-model-and-features-store ##	Purges the store containing prediction medels and host model features on startup, so that it's guaranteed to be using fresh data. ##

--purge-optimization-guide-store ##	Purges the store containing fetched and component hints on startup, so that it's guaranteed to be using fresh data. ##

--pwa-launcher-version[1] ##	Specifies the version of the Progressive-Web-App launcher that launched Chrome, used to determine whether to update all launchers. NOTE: changing this switch requires adding legacy handling for the previous method, as older PWA launchers still using this switch will rely on Chrome to update them to use the new method. ##

--quarantine-dir ##	Specifies the quarantine folder instead of the default one. ##

--query-tiles-country-code ##	No description ##

--query-tiles-instant-background-task ##	No description ##

--query-tiles-single-tier ##	No description ##

--rdp\_desktop\_session ##	No description ##

--reached-code-sampling-interval-us[7] ##	Specifies the profiling interval in microseconds for reached code profiler. ##

--reader-mode-feedback ##	No description ##

--reader-mode-heuristics ##	No description ##

--realtime-reporting-url ##	Specifies the URL at which to upload real-time reports. ##

--redirect-libassistant-logging ##	Redirects libassistant logging to /var/log/chrome/. ##

--register-font-files[1] ##	Registers additional font files on Windows (for fonts outside the usual %WINDIR%\Fonts location). Multiple files can be used by separating them with a semicolon (;). ##

--register-max-dark-suspend-delay ##	Makes Chrome register the maximum dark suspend delay possible on Chrome OS i.e. give the device the maximum amount of time to do its work in dark resume as is allowed by the power manager. ##

--register-pepper-plugins ##	Register Pepper plugins (see pepper\_plugin\_list.cc for its format). ##

--regulatory-label-dir ##	The name of the per-model directory which contains per-region subdirectories with regulatory label files for this model. The per-model directories (if there are any) are located under "/usr/share/chromeos-assets/regulatory\_labels/". ##

--relauncher[6] ##	A process type (switches::kProcessType) that relaunches the browser. See chrome/browser/mac/relauncher.h. ##

--remote-debugger-handles ##	No description ##

--remote-debugging-address ##	Use the given address instead of the default loopback for accepting remote debugging connections. Should be used together with --remote-debugging-port. Note that the remote debugging protocol does not perform any authentication, so exposing it too widely can be a security risk. ##

--remote-debugging-pipe ##	Enables remote debug over stdio pipes [in=3, out=4]. Optionally, specifies the format for the protocol messages, can be either "JSON" (the default) or "CBOR". ##

--remote-debugging-port ##	Enables remote debug over HTTP on the specified port. ##

--remote-debugging-socket-name[7] ##	Enables remote debug over HTTP on the specified socket name. ##

--remote-debugging-targets ##	Provides a list of addresses to discover DevTools remote debugging targets. The format is <host>:<port>,...,<host>:port. ##

--remove-scan-only-uws ##	Allow the engine to remove UwS that isn't marked cleanable. For testing only. ##

--renderer ##	No description ##

--renderer-client-id ##	No description ##

--renderer-cmd-prefix ##	The contents of this flag are prepended to the renderer command line. Useful values might be "valgrind" or "xterm -e gdb --args". ##

--renderer-process-limit ##	Overrides the default/calculated limit to the number of renderer processes. Very high values for this setting can lead to high memory/resource usage or instability. ##

--renderer-sampling ##	No description ##

--renderer-startup-dialog ##	Causes the renderer process to display a dialog on launch. Passing this flag also adds sandbox::policy::kNoSandbox on Windows non-official builds, since that's needed to show a dialog. ##

--renderer-wait-for-java-debugger[7] ##	Block ChildProcessMain thread of the renderer's ChildProcessService until a Java debugger is attached. ##

--renderpass ##	Parameters for kUIShowCompositedLayerBorders. ##

--repl ##	Runs a read-eval-print loop that allows the user to evaluate Javascript expressions. ##

--report-vp9-as-an-unsupported-mime-type ##	Force to report VP9 as an unsupported MIME type. ##

--require-audio-hardware-for-testing ##	When running tests on a system without the required hardware or libraries, this flag will cause the tests to fail. Otherwise, they silently succeed. ##

--require-wlan ##	Only connect to WLAN interfaces. ##

--reset-variation-state ##	Forces a reset of the one-time-randomized FieldTrials on this client, also known as the Chrome Variations state. ##

--restore-last-session ##	Indicates the last session should be restored on startup. This overrides the preferences value. Note that this does not force automatic session restore following a crash, so as to prevent a crash loop. This switch is used to implement support for OS-specific "continue where you left off" functionality on OS X and Windows. ##

--restrict-gamepad-access ##	Enables Feature Policy and Secure Context requirements on getGamepads. ##

--rlz-ping-delay ##	The rlz ping delay (in seconds) that overwrites the default value. ##

--rtl ##	No description ##

--run-all-compositor-stages-before-draw ##	Effectively disables pipelining of compositor frame production stages by waiting for each stage to finish before completing a frame. ##

--run-manual ##	Manual tests only run when --run-manual is specified. This allows writing tests that don't run automatically but are still in the same test binary. This is useful so that a team that wants to run a few tests doesn't have to add a new binary that must be compiled on all builds. ##

--run-web-tests ##	Runs Content Shell in web test mode, injecting test-only behaviour for blink web tests. ##

--run-without-sandbox-for-testing[3] ##	Load the engine outside the sandbox. This is only to be used for manual testing. ##

--safe-mode ##	The switch added by session\_manager daemon when chrome crashes 3 times or more within the first 60 seconds on start. See BrowserJob::ExportArgv in platform2/login\_manager/browser\_job.cc. ##

--safebrowsing-enable-enhanced-protection ##	Enable Safe Browsing Enhanced Protection. ##

--safebrowsing-manual-download-blacklist ##	List of comma-separated sha256 hashes of executable files which the download-protection service should treat as "dangerous." For a file to show a warning, it also must be considered a dangerous filetype and not be whitelisted otherwise (by signature or URL) and must be on a supported OS. Hashes are in hex. This is used for manual testing when looking for ways to by-pass download protection. ##

--SafeSites ##	No description ##

--saml-password-change-url ##	Password change url for SAML users. TODO(941489): Remove when the bug is fixed. ##

--sandbox-ipc ##	Causes the process to run as a sandbox IPC subprocess. ##

--sandbox-mojo-pipe-token ##	Mojo pipe token generated in the broker process and passed to the sandbox process to bind with the EngineCommands IPC interface. ##

--sandboxed-process-id ##	Used to identify the id of the sandbox process that is intended to be spawned. ##

--save-page-as-mhtml ##	Disable saving pages as HTML-only, disable saving pages as HTML Complete (with a directory of sub-resources). Enable only saving pages as MHTML. See <http://crbug.com/120416> for how to remove this switch. ##

--scan-locations ##	Used to limit trace locations that will be scanned. Trace locations should be specified as integers, separated by commas. For example: --scan-locations=1,2,3,5 ##

--scanning-timeout ##	Set the timeout for the scanning phase, in minutes. 0 disables the timeout entirely. WARNING: this switch is used by internal test systems. Be careful when making changes. ##

--scheduler-configuration[5] ##	Selects the scheduler configuration specified in the parameter. ##

--scheduler-configuration-default[5] ##	Specifies what the default scheduler configuration value is if the user does not set one. ##

--screen-config ##	Specifies the initial screen configuration, or state of all displays, for FakeDisplayDelegate, see class for format details. ##

--screenshot ##	Save a screenshot of the loaded page. ##

--search-provider-logo-url ##	Use a static URL for the logo of the default search engine. Example: <https://www.google.com/branding/logo.png> ##

--secondary-display-layout ##	Specifies the layout mode and offsets for the secondary display for testing. The format is "<t|r|b|l>,<offset>" where t=TOP, r=RIGHT, b=BOTTOM and L=LEFT. For example, 'r,-100' means the secondary display is positioned on the right with -100 offset. (above than primary) ##

--service ##	No description ##

--service-manager ##	The value of the |kProcessType| switch which tells the executable to assume the role of a standalone Service Manager instance. ##

--service-name ##	Indicates the name of the service to run. Useful for debugging, or if a service executable is built to support being run as a number of potential different services. ##

--service-request-attachment-name ##	The name of the |mojo::PendingReceiver<service\_manager::mojom::Service>| message pipe handle that is attached to the incoming Mojo invitation received by the service. ##

--service-request-channel-token ##	The token to use to construct the message pipe for a service in a child process. ##

--service-runner ##	The value of the |kProcessType| switch which tells the executable to assume the role of a service instance. ##

--service-sandbox-type ##	Type of sandbox to apply to the process running the service, one of the values in the next block. ##

--set-extension-throttle-test-params ##	Set the parameters for ExtensionURLLoaderThrottleBrowserTest. ##

--shader-disk-cache-size-kb ##	Allows explicitly specifying the shader disk cache size for embedded devices. Default value is 6MB. On Android, 2MB is default and 128KB for low-end devices. ##

--shared-files ##	Describes the file descriptors passed to a child process in the following list format: <file\_id>:<descriptor\_id>,<file\_id>:<descriptor\_id>,... where <file\_id> is an ID string from the manifest of the service being launched and <descriptor\_id> is the numeric identifier of the descriptor for the child process can use to retrieve the file descriptor from the global descriptor table. ##

--sharing\_service ##	No description ##

--shelf-hotseat ##	New modular design for the shelf with apps separated into a hotseat UI and smaller shelf in clamshell mode. ##

--shelf-hover-previews ##	App window previews when hovering over the shelf. ##

--shill-stub ##	Overrides Shill stub behavior. By default, ethernet, wifi and vpn are enabled, and transitions occur instantaneously. Multiple options can be comma separated (no spaces). Note: all options are in the format 'foo=x'. Values are case sensitive and based on Shill names in service\_constants.h. See FakeShillManagerClient::SetInitialNetworkState for implementation. Examples: 'clear=1' - Clears all default configurations 'wifi=on' - A wifi network is initially connected ('1' also works) 'wifi=off' - Wifi networks are all initially disconnected ('0' also works) 'wifi=disabled' - Wifi is initially disabled 'wifi=none' - Wifi is unavailable 'wifi=portal' - Wifi connection will be in Portal state 'cellular=1' - Cellular is initially connected 'cellular=LTE' - Cellular is initially connected, technology is LTE 'interactive=3' - Interactive mode, connect/scan/etc requests take 3 secs ##

--short-merge-session-timeout-for-test[5] ##	Use a short (1 second) timeout for merge session loader throttle testing. ##

--short-reporting-delay ##	Sets the Reporting API delay to under a second to allow much quicker reports. ##

--show-aggregated-damage ##	Adds a DebugBorderDrawQuad to the top of the root RenderPass showing the damage rect after surface aggregation. Note that when enabled this feature sets the entire output rect as damaged after adding the quad to highlight the real damage rect, which could hide damage rect problems. ##

--show-android-files-in-files-app ##	If true, files in Android internal storage will be shown in Files app. ##

--show-auth-dialog-dev-overlay ##	If true, the debug view overlay will be shown for the in-session authentication dialog. This enables development of the dialog. TODO(yichenli): Remove this after the feature is released. ##

--show-autofill-signatures ##	Annotates forms and fields with Autofill signatures. ##

--show-autofill-type-predictions ##	Annotates forms with Autofill field type predictions. ##

--show-component-extension-options ##	Makes component extensions appear in chrome://settings/extensions. ##

--show-composited-layer-borders ##	Renders a border around compositor layers to help debug and study layer compositing. ##

--show-dc-layer-debug-borders ##	Show debug borders for DC layers - red for overlays and blue for underlays. The debug borders are offset from the layer rect by a few pixels for clarity. ##

--show-fps-counter ##	Draws a heads-up-display showing Frames Per Second as well as GPU memory usage. If you also use --enable-logging=stderr --vmodule="head\*=1" then FPS will also be output to the console log. ##

--show-icons[1] ##	See kHideIcons. ##

--show-layer-animation-bounds ##	Renders a border that represents the bounding box for the layer's animation. ##

--show-layout-shift-regions ##	Visibly render a border around layout shift rects in the web page to help debug and study layout shifts. ##

--show-login-dev-overlay ##	If true, the developer tool overlay will be shown for the login/lock screen. This makes it easier to test layout logic. ##

--show-mac-overlay-borders[6] ##	Show borders around CALayers corresponding to overlays and partial damage. ##

--show-oobe-dev-overlay ##	Enables OOBE UI Debugger for ease of navigation between screens during manual testing. Limited to ChromeOS-on-linux and test images only. ##

--show-overdraw-feedback ##	Visualize overdraw by color-coding elements based on if they have other elements drawn underneath. This is good for showing where the UI might be doing more rendering work than necessary. The colors are hinting at the amount of overdraw on your screen for each pixel, as follows: True color: No overdraw. Blue: Overdrawn once. Green: Overdrawn twice. Pink: Overdrawn three times. Red: Overdrawn four or more times. ##

--show-paint-rects ##	Visibly render a border around paint rects in the web page to help debug and study painting behavior. ##

--show-property-changed-rects ##	Show rects in the HUD around layers whose properties have changed. ##

--show-screenspace-rects ##	Show rects in the HUD around the screen-space transformed bounds of every layer. ##

--show-surface-damage-rects ##	Show rects in the HUD around damage as it is recorded into each render surface. ##

--show-taps ##	Draws a circle at each touch point, similar to the Android OS developer option "Show taps". ##

--silent-debugger-extension-api ##	Does not show an infobar when an extension attaches to a page using chrome.debugger page. Required to attach to extension background pages. ##

--silent-launch ##	Causes Chrome to launch without opening any windows by default. Useful if one wishes to use Chrome as an ash server. ##

--simulate-critical-update ##	Simulates a critical update being available. ##

--simulate-elevated-recovery ##	Simulates that elevation is needed to recover upgrade channel. ##

--simulate-outdated ##	Simulates that current version is outdated. ##

--simulate-outdated-no-au ##	Simulates that current version is outdated and auto-update is off. ##

--simulate-update-error-code ##	Simulates a GoogleUpdateErrorCode error by the update check. Must be supplied with |kSimulateUpdateHresult| switch. ##

--simulate-update-hresult ##	Simulates a specific HRESULT error code returned by the update check. If the switch value is not specified (as hex) then it defaults to E\_FAIL. ##

--simulate-upgrade ##	Simulates an update being available. ##

--single-process ##	Runs the renderer and plugins in the same process as the browser ##

--site-per-process ##	Enforces a one-site-per-process security policy: \* Each renderer process, for its whole lifetime, is dedicated to rendering pages for just one site. \* Thus, pages from different sites are never in the same process. \* A renderer process's access rights are restricted based on its site. \* All cross-site navigations force process swaps. \* <iframe>s are rendered out-of-process whenever the src= is cross-site. More details here: - <https://www.chromium.org/developers/design-documents/site-isolation> - <https://www.chromium.org/developers/design-documents/process-models> - The class comment in site\_instance.h, listing the supported process models. IMPORTANT: this isn't to be confused with --process-per-site (which is about process consolidation, not isolation). You probably want this one. ##

--skia-font-cache-limit-mb ##	Specifies the max number of bytes that should be used by the skia font cache. If the cache needs to allocate more, skia will purge previous entries. ##

--skia-resource-cache-limit-mb ##	Specifies the max number of bytes that should be used by the skia resource cache. The previous entries are purged from the cache when the memory usage exceeds this limit. ##

--slow-down-compositing-scale-factor ##	Re-draw everything multiple times to simulate a much slower machine. Give a slow down factor to cause renderer to take that many times longer to complete, such as --slow-down-compositing-scale-factor=2. ##

--slow-down-raster-scale-factor ##	Re-rasters everything multiple times to simulate a much slower machine. Give a scale factor to cause raster to take that many times longer to complete, such as --slow-down-raster-scale-factor=25. ##

--sms-test-messages ##	Sends test messages on first call to RequestUpdate (stub only). ##

--spdy-proxy-auth-fallback ##	The origin of the data reduction proxy fallback. ##

--spdy-proxy-auth-origin ##	The origin of the data reduction proxy. ##

--spdy-proxy-auth-value ##	A test key for data reduction proxy authentication. ##

--speech\_recognition ##	No description ##

--ssl-key-log-file ##	Causes SSL key material to be logged to the specified file for debugging purposes. See <https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS/Key_Log_Format> for the format. ##

--ssl-version-max ##	Specifies the maximum SSL/TLS version ("tls1.2" or "tls1.3"). ##

--ssl-version-min ##	Specifies the minimum SSL/TLS version ("tls1", "tls1.1", "tls1.2", or "tls1.3"). ##

--stable-release-mode ##	This makes us disable some web-platform runtime features so that we test content\_shell as if it was a stable release. It is only followed when kRunWebTest is set. For the features' level, see <http://dev.chromium.org/blink/runtime-enabled-features>. ##

--staging ##	The command line alias and URL for the "staging" environment. ##

--start-fullscreen ##	Specifies if the browser should start in fullscreen mode, like if the user had pressed F11 right after startup. ##

--start-in-incognito ##	Starts the shell with the profile in incognito mode. ##

--start-maximized ##	Starts the browser maximized, regardless of any previous settings. ##

--start-stack-profiler ##	Starts the stack sampling profiler in the child process. ##

--started ##	Value for kTestCrosGaiaIdMigration indicating that migration is started (i.e. all stored user keys will be converted to GaiaId) ##

--storage-pressure-notification-interval ##	Interval, in minutes, used for storage pressure notification throttling. Useful for developers testing applications that might use non-trivial amounts of disk space. ##

--structured-metrics-disabled ##	Disable structured metrics logging of CrOS actions. ##

--stub ##	No description ##

--suppress-message-center-popups ##	Hides all Message Center notification popups (toasts). Used for testing. ##

--surface ##	No description ##

--swiftshader ##	No description ##

--swiftshader-webgl ##	No description ##

--sync-deferred-startup-timeout-seconds ##	Allows overriding the deferred init fallback timeout. ##

--sync-disable-deferred-startup ##	Enables deferring sync backend initialization until user initiated changes occur. ##

--sync-include-specifics ##	Controls whether the initial state of the "Capture Specifics" flag on chrome://sync-internals is enabled. ##

--sync-short-initial-retry-override ##	This flag causes sync to retry very quickly (see polling\_constants.h) the when it encounters an error, as the first step towards exponential backoff. ##

--sync-short-nudge-delay-for-test ##	This flag significantly shortens the delay between nudge cycles. Its primary purpose is to speed up integration tests. The normal delay allows coalescing and prevention of server overload, so don't use this unless you're really sure that it's what you want. ##

--sync-url ##	Overrides the default server used for profile sync. ##

--sys-info-file-path ##	System info file path. Default is an empty string, which means that dummy info will be used. ##

--system-developer-mode ##	Used by FakeDebugDaemonClient to specify that the system is running in dev mode when running in a Linux environment. The dev mode probing is done by session manager. ##

--system-gesture-start-height ##	The number of pixels from the very top or bottom of the screen to consider as a valid origin for the top or bottom swipe gesture. Overrides the default value in cast\_system\_gesture\_handler.cc. ##

--system-gesture-start-width ##	The number of pixels from the very left or right of the screen to consider as a valid origin for the left or right swipe gesture. Overrides the default value in cast\_system\_gesture\_handler.cc. ##

--system-log-upload-frequency ##	Frequency in Milliseconds for system log uploads. Should only be used for testing purposes. ##

--telemetry-extension-dir ##	Specifies directory for the Telemetry System Web Extension. ##

--test-child-process ##	When running certain tests that spawn child processes, this switch indicates to the test framework that the current process is a child process. ##

--test-cros-gaia-id-migration ##	Controls CrOS GaiaId migration for tests ("" is default). ##

--test-do-not-initialize-icu ##	When running certain tests that spawn child processes, this switch indicates to the test framework that the current process should not initialize ICU to avoid creating any scoped handles too early in startup. ##

--test-encryption-migration-ui ##	Enables testing for encryption migration UI. ##

--test-gl-lib ##	Flag used for Linux tests: for desktop GL bindings, try to load this GL library first, but fall back to regular library if loading fails. ##

--test-logging-path ##	Set a path to save logs in while testing. ##

--test-logging-url ##	Set a test logging URL, where logs will be uploaded. ##

--test-name ##	Passes the name of the current running automated test to Chrome. ##

--test-type ##	Type of the current test harness ("browser" or "ui"). ##

--test-wallpaper-server ##	Enables the wallpaper picker to fetch images from the test server. ##

--testing ##	Flag set during testing and stored as a crash key, to differentiate from crashes received from actual users. WARNING: this switch is used by internal test systems. Be careful when making changes. ##

--tether-host-scans-ignore-wired-connections ##	Tells the Chromebook to scan for a tethering host even if there is already a wired connection. This allows end-to-end tests to be deployed over ethernet without that connection preventing scans and thereby blocking the testing of cases with no preexisting connection. Should be used only for testing. ##

--tether-stub ##	Overrides Tether with stub service. Provide integer arguments for the number of fake networks desired, e.g. 'tether-stub=2'. ##

--third-party-doodle-url ##	Overrides the Doodle URL to use for third-party search engines. Testing? Try: <https://www.gstatic.com/chrome/ntp/doodle_test/third_party_simple.json> <https://www.gstatic.com/chrome/ntp/doodle_test/third_party_animated.json> ##

--timeout ##	Issues a stop after the specified number of milliseconds. This cancels all navigation and causes the DOMContentLoaded event to fire. ##

--tint-composited-content ##	Tint composited color. ##

--tls1 ##	TLS 1.0 mode for the |kSSLVersionMin| switch. ##

--tls1.1 ##	TLS 1.1 mode for the |kSSLVersionMin| switch. ##

--tls1.2 ##	TLS 1.2 mode for |kSSLVersionMax| and |kSSLVersionMin| switches. ##

--tls1.3 ##	TLS 1.3 mode for |kSSLVersionMax| and |kSSLVersionMin| switches. ##

--top-chrome-touch-ui ##	Controls touch-optimized UI layout for top chrome. ##

--top-controls-hide-threshold ##	Percentage of the browser controls need to be hidden before they will auto hide. ##

--top-controls-show-threshold ##	Percentage of the browser controls need to be shown before they will auto show. ##

--touch-devices[10] ##	Tells chrome to interpret events from these devices as touch events. Only available with XInput 2 (i.e. X server 1.8 or above). The id's of the devices can be retrieved from 'xinput list'. ##

--touch-events ##	Enable support for touch event feature detection. ##

--touch-selection-strategy ##	Controls how text selection granularity changes when touch text selection handles are dragged. Should be "character" or "direction". If not specified, the platform default is used. ##

--touchscreen-usable-while-screen-off ##	Chromebases' touchscreens can be used to wake from suspend, unlike the touchscreens on other Chrome OS devices. If set, the touchscreen is kept enabled while the screen is off so that it can be used to turn the screen back on after it has been turned off for inactivity but before the system has suspended. ##

--touch\_view ##	No description ##

--trace-config-file ##	Causes TRACE\_EVENT flags to be recorded from startup. This flag will be ignored if --trace-startup or --trace-shutdown is provided. ##

--trace-startup ##	Causes TRACE\_EVENT flags to be recorded from startup. Optionally, can specify the specific trace categories to include (e.g. --trace-startup=base,net) otherwise, all events are recorded. Setting this flag results in the first call to BeginTracing() to receive all trace events since startup. In Chrome, you may find --trace-startup-file and --trace-startup-duration to control the auto-saving of the trace (not supported in the base-only TraceLog component). ##

--trace-startup-duration ##	Sets the time in seconds until startup tracing ends. If omitted a default of 5 seconds is used. Has no effect without --trace-startup, or if --startup-trace-file=none was supplied. ##

--trace-startup-enable-privacy-filtering ##	If the perfetto tracing backend is used, this enables privacy filtering in the TraceEvent data sources for the startup tracing session. ##

--trace-startup-file ##	If supplied, sets the file which startup tracing will be stored into, if omitted the default will be used "chrometrace.log" in the current directory. Has no effect unless --trace-startup is also supplied. Example: --trace-startup --trace-startup-file=/tmp/trace\_event.log As a special case, can be set to 'none' - this disables automatically saving the result to a file and the first manually recorded trace will then receive all events since startup. ##

--trace-startup-owner ##	Specifies the coordinator of the startup tracing session. If the legacy tracing backend is used instead of perfetto, providing this flag is not necessary. Valid values: 'controller', 'devtools', or 'system'. Defaults to 'controller'. If 'controller' is specified, the session is controlled and stopped via the TracingController (e.g. to implement the timeout). If 'devtools' is specified, the startup tracing session will be owned by DevTools and thus can be controlled (i.e. stopped) via the DevTools Tracing domain on the first session connected to the browser endpoint. If 'system' is specified, the system Perfetto service should already be tracing on a supported platform (currently only Android). Session is stopped through the normal methods for stopping system traces. ##

--trace-startup-record-mode ##	If supplied, sets the tracing record mode and options; otherwise, the default "record-until-full" mode will be used. ##

--trace-to-console ##	Sends a pretty-printed version of tracing info to the console. ##

--trace-to-file ##	Sends trace events from these categories to a file. --trace-to-file on its own sends to default categories. ##

--trace-to-file-name ##	Specifies the file name for --trace-to-file. If unspecified, it will go to a default file name. ##

--trace-upload-url ##	Sets the target URL for uploading tracing data. ##

--translate-ranker-model-url ##	Overrides the URL from which the translate ranker model is downloaded. ##

--translate-script-url ##	Overrides the default server used for Google Translate. ##

--translate-security-origin ##	Overrides security-origin with which Translate runs in an isolated world. ##

--true ##	Value indicating whether flag from command line switch is true. ##

--trustable-web-bundles-file-url ##	Accepts specified file URL of a trustable WebBundle file. This flag should be used only for testing purpose. ##

--trusted-cdn-base-url-for-tests[7] ##	Specifies a base URL for the trusted CDN for tests. ##

--trusted-download-sources ##	Identifies a list of download sources as trusted, but only if proper group policy is set. ##

--try-chrome-again ##	Experimental. Shows a dialog asking the user to try chrome. This flag is to be used only by the upgrade process. ##

--try-supported-channel-layouts[1] ##	Instead of always using the hardware channel layout, check if a driver supports the source channel layout. Avoids outputting empty channels and permits drivers to enable stereo to multichannel expansion. Kept behind a flag since some drivers lie about supported layouts and hang when used. See <http://crbug.com/259165> for more details. ##

--tts[5] ##	No description ##

--type ##	Indicates the type of process to run. This may be "service-manager", "service-runner", or any other arbitrary value supported by the embedder. ##

--ui-compositor-memory-limit-when-visible-mb ##	No description ##

--ui-disable-partial-swap ##	Disable partial swap which is needed for some OpenGL drivers / emulators. ##

--ui-disable-zero-copy ##	No description ##

--ui-enable-layer-lists ##	Switches the ui compositor to use layer lists instead of layer trees. ##

--ui-enable-rgba-4444-textures ##	No description ##

--ui-enable-zero-copy ##	No description ##

--ui-show-composited-layer-borders ##	No description ##

--ui-show-fps-counter ##	No description ##

--ui-show-layer-animation-bounds ##	No description ##

--ui-show-paint-rects ##	No description ##

--ui-show-property-changed-rects ##	No description ##

--ui-show-screenspace-rects ##	No description ##

--ui-show-surface-damage-rects ##	No description ##

--ui-slow-animations ##	No description ##

--unfiltered-bluetooth-devices ##	Shows all Bluetooth devices in UI (System Tray/Settings Page.) ##

--uninstall[1] ##	Runs un-installation steps that were done by chrome first-run. ##

--unlimited-storage ##	Overrides per-origin quota settings to unlimited storage for any apps/origins. This should be used only for testing purpose. ##

--unsafely-allow-protected-media-identifier-for-domain ##	For automated testing of protected content, this switch allows specific domains (e.g. example.com) to skip asking the user for permission to share the protected media identifier. In this context, domain does not include the port number. User's content settings will not be affected by enabling this switch. Reference: <http://crbug.com/718608> Example: --unsafely-allow-protected-media-identifier-for-domain=a.com,b.ca ##

--unsafely-treat-insecure-origin-as-secure ##	Treat given (insecure) origins as secure origins. Multiple origins can be supplied as a comma-separated list. For the definition of secure contexts, see <https://w3c.github.io/webappsec-secure-contexts/> and <https://www.w3.org/TR/powerful-features/#is-origin-trustworthy> Example: --unsafely-treat-insecure-origin-as-secure=http://a.test,http://b.test ##

--upload-log-file ##	Specifies the full path to a protocol buffer log file to be uploaded. ##

--use-adapter-luid ##	Initialize the GPU process using the adapter with the specified LUID. This is only used on Windows, as LUID is a Windows specific structure. ##

--use-angle ##	Select which ANGLE backend to use. Options are: default: Attempts several ANGLE renderers until one successfully initializes, varying ES support by platform. d3d9: Legacy D3D9 renderer, ES2 only. d3d11: D3D11 renderer, ES2 and ES3. warp: D3D11 renderer using software rasterization, ES2 and ES3. gl: Desktop GL renderer, ES2 and ES3. gles: GLES renderer, ES2 and ES3. ##

--use-cmd-decoder ##	Use the Pass-through command decoder, skipping all validation and state tracking. Switch lives in ui/gl because it affects the GL binding initialization on platforms that would otherwise not default to using EGL bindings. ##

--use-cras[20] ##	Use CRAS, the ChromeOS audio server. ##

--use-crash-handler-in-tests ##	If present, the test harness will use the crash reporter. ##

--use-crash-handler-with-id ##	Specifies the IPC pipe name of the crash handler to use (instead of starting a new crash handler process). ##

--use-fake-codec-for-peer-connection ##	Replaces the existing codecs supported in peer connection with a single fake codec entry that create a fake video encoder and decoder. ##

--use-fake-device-for-media-stream ##	Use fake device for Media Stream to replace actual camera and microphone. For the list of allowed parameters, see FakeVideoCaptureDeviceFactory::ParseFakeDevicesConfigFromOptionsString(). ##

--use-fake-mjpeg-decode-accelerator ##	Use a fake device for accelerated decoding of MJPEG. This allows, for example, testing of the communication to the GPU service without requiring actual accelerator hardware to be present. ##

--use-fake-ui-for-media-stream ##	Bypass the media stream infobar by selecting the default device for media streams (e.g. WebRTC). Works with --use-fake-device-for-media-stream. ##

--use-file-for-fake-audio-capture ##	Play a .wav file as the microphone. Note that for WebRTC calls we'll treat the bits as if they came from the microphone, which means you should disable audio processing (lest your audio file will play back distorted). The input file is converted to suit Chrome's audio buses if necessary, so most sane .wav files should work. You can pass either <path> to play the file looping or <path>%noloop to stop after playing the file to completion. ##

--use-file-for-fake-video-capture ##	Use an .y4m file to play as the webcam. See the comments in media/capture/video/file\_video\_capture\_device.h for more details. ##

--use-first-display-as-internal ##	Uses the 1st display in --ash-host-window-bounds as internal display. This is for debugging on linux desktop. ##

--use-gl ##	Select which implementation of GL the GPU process should use. Options are: desktop: whatever desktop OpenGL the user has installed (Linux and Mac default). egl: whatever EGL / GLES2 the user has installed (Windows default - actually ANGLE). swiftshader: The SwiftShader software renderer. ##

--use-gpu-high-thread-priority-for-perf-tests ##	Increases the priority (to REALTIME\_AUDIO) of gpu process and compositor thread. This is only to be used for perf tests on macOS for more reliable values. ##

--use-gpu-in-tests ##	Use hardware gpu, if available, for tests. ##

--use-legacy-metrics-service ##	No description ##

--use-mobile-user-agent ##	Set when Chromium should use a mobile user agent. ##

--use-mock-cert-verifier-for-testing ##	Use the MockCertVerifier. This only works in test code. ##

--use-mock-keychain[8] ##	No description ##

--use-system-clipboard ##	Enables the ozone x11 clipboard for linux-chromeos. ##

--use-system-default-printer[21] ##	Uses the system default printer as the initially selected destination in print preview, instead of the most recently used destination. ##

--use-temp-registry-path ##	Override the registry with the specified temporary registry. Used for tests. ##

--use-vulkan ##	Enable Vulkan support and select Vulkan implementation, must also have ENABLE\_VULKAN defined. This only initializes Vulkan, the flag --enable-features=Vulkan must also be used to select Vulkan for compositing and rasterization. ##

--user-agent ##	A string used to override the default user agent with a custom one. ##

--user-agent-product ##	No description ##

--user-always-affiliated ##	Always treat user as affiliated. TODO(antrim): Remove once test servers correctly produce affiliation ids. ##

--user-data-dir ##	Directory where the browser stores the user profile. ##

--user-data-migrated[22] ##	Indicates that this process is the product of a relaunch following migration of User Data. ##

--user-gesture-required ##	Autoplay policy to require a user gesture in order to play. ##

--user-response-timeout ##	Set the timeout for how long to wait for user response from Chrome, in minutes. 0 disables the timeout entirely. ##

--utility ##	No description ##

--utility-and-browser ##	No description ##

--utility-cmd-prefix ##	The contents of this flag are prepended to the utility process command line. Useful values might be "valgrind" or "xterm -e gdb --args". ##

--utility-sampling ##	No description ##

--utility-startup-dialog ##	Causes the utility process to display a dialog on launch. ##

--utility-sub-type ##	This switch indicates the type of a utility process. It is not used by Chrome but is added to the command line for debugging and profiling purposes. ##

--v ##	Gives the default maximal active V-logging level; 0 is the default. Normally positive values are used for V-logging levels. ##

--v8-cache-options ##	Set options to cache V8 data. (off, preparse data, or code) ##

--validate-crx ##	Examines a .crx for validity and prints the result. ##

--validate-input-event-stream ##	In debug builds, asserts that the stream of input events is valid. ##

--validating ##	The command decoder names that can be passed to --use-cmd-decoder. ##

--variations-insecure-server-url ##	Specifies a custom URL for the server to use as an insecure fallback when requests to |kVariationsServerURL| fail. Requests to this URL will be encrypted. ##

--variations-override-country ##	Allows overriding the country used for evaluating variations. This is similar to the "Override Variations Country" entry on chrome://translate-internals, but is exposed as a command-line flag to allow testing First Run scenarios. Additionally, unlike chrome://translate-internals, the value isn't persisted across sessions. ##

--variations-server-url ##	Specifies a custom URL for the server which reports variation data to the client. Specifying this switch enables the Variations service on unofficial builds. See variations\_service.cc. ##

--version ##	No description ##

--video-capture-use-gpu-memory-buffer ##	Enables GpuMemoryBuffer-based buffer pool. ##

--video-image-texture-target ##	Texture target for CHROMIUM\_image backed video frame textures. ##

--video-threads ##	Set number of threads to use for video decoding. ##

--video-underflow-threshold-ms ##	Allows clients to override the threshold for when the media renderer will declare the underflow state for the video stream when audio is present. TODO(dalecurtis): Remove once experiments for <http://crbug.com/470940> finish. ##

--video\_capture ##	No description ##

--virtual-time-budget ##	If set the system waits the specified number of virtual milliseconds before deeming the page to be ready. For determinism virtual time does not advance while there are pending network fetches (i.e no timers will fire). Once all network fetches have completed, timers fire and if the system runs out of virtual time is fastforwarded so the next timer fires immediately, until the specified virtual time budget is exhausted. ##

--vmodule ##	Gives the per-module maximal V-logging levels to override the value given by --v. E.g. "my\_module=2,foo\*=3" would change the logging level for all code in source files "my\_module.\*" and "foo\*.\*" ("-inl" suffixes are also disregarded for this matching). Any pattern containing a forward or backward slash will be tested against the whole pathname and not just the module. E.g., "\*/foo/bar/\*=2" would change the logging level for all code in source files under a "foo/bar" directory. ##

--vsync-interval ##	Overrides the vsync interval used by the GPU process to refresh the display. ##

--vulkan ##	No description ##

--vulkan-null ##	No description ##

--wait-for-debugger ##	Will wait for 60 seconds for a debugger to come to attach to the process. ##

--wait-for-debugger-children ##	Will add kWaitForDebugger to every child processes. If a value is passed, it will be used as a filter to determine if the child process should have the kWaitForDebugger flag passed on or not. ##

--wait-for-debugger-webui ##	Flag used by WebUI test runners to wait for debugger to be attached. ##

--wait-for-initial-policy-fetch-for-test ##	Used to tell the policy infrastructure to not let profile initialization complete until policy is manually set by a test. This is used to provide backward compatibility with a few tests that incorrectly use the synchronously-initialized login profile to run their tests - do not add new uses of this flag. ##

--wallet-service-use-sandbox ##	Use the sandbox Online Wallet service URL (for developer testing). ##

--waveout-buffers[1] ##	Number of buffers to use for WaveOut. ##

--web-otp-backend ##	Enables specified backend for the Web OTP API. ##

--web-otp-backend-sms-verification ##	Enables Sms Verification backend for Web OTP API which requires app hash in SMS body. ##

--web-otp-backend-user-consent ##	Enables User Consent backend for Web OTP API. ##

--webapk-server-url[7] ##	Custom WebAPK server URL for the sake of testing. ##

--webgl-antialiasing-mode ##	Set the antialiasing method used for webgl. (none, explicit, implicit) ##

--webgl-msaa-sample-count ##	Set a default sample count for webgl if msaa is enabled. ##

--weblayer-user-data-dir ##	Makes WebLayer Shell use the given path for its data directory. ##

--webrtc-event-log-proactive-pruning-delta ##	Sets the delay (in seconds) between proactive prunings of remote-bound WebRTC event logs which are pending upload. All positive values are legal. All negative values are illegal, and ignored. If set to 0, the meaning is "no proactive pruning". ##

--webrtc-event-log-upload-delay-ms ##	WebRTC event logs will only be uploaded if the conditions hold for this many milliseconds. ##

--webrtc-event-log-upload-no-suppression ##	Normally, remote-bound WebRTC event logs are uploaded only when no peer connections are active. With this flag, the upload is never suppressed. ##

--webrtc-event-logging ##	Enable capture and local storage of WebRTC event logs without visiting chrome://webrtc-internals. This is useful for automated testing. It accepts the path to which the local logs would be stored. Disabling is not possible without restarting the browser and relaunching without this flag. ##

--webrtc-max-cpu-consumption-percentage ##	Configure the maximum CPU time percentage of a single core that can be consumed for desktop capturing. Default is 50. Set 100 to disable the throttling of the capture. ##

--webrtc-stun-probe-trial ##	Renderer process parameter for WebRTC Stun probe trial to determine the interval. Please see SetupStunProbeTrial in chrome\_browser\_field\_trials\_desktop.cc for more detail. ##

--webview-disable-safebrowsing-support ##	used to disable safebrowsing functionality in webview ##

--webview-enable-vulkan ##	Used to enable vulkan draw mode instead of interop draw mode for webview. ##

--webview-log-js-console-messages ##	No description ##

--webview-safebrowsing-block-all-resources ##	Enables SafeBrowsing and causes WebView to treat all resources as malicious. Use care: this will block all resources from loading. ##

--webview-sandboxed-renderer ##	No description ##

--webview-verbose-logging ##	WebView will log additional debugging information to logcat, such as variations and commandline state. ##

--whitelisted-extension-id ##	Adds the given extension ID to all the permission allowlists. ##

--win-jumplist-action ##	Specifies which category option was clicked in the Windows Jumplist that resulted in a browser startup. ##

--window-position ##	Specify the initial window position: --window-position=x,y ##

--window-size ##	Sets the initial window size. Provided as string in the format "800,600". ##

--window-workspace ##	Specify the initial window workspace: --window-workspace=id ##

--windows-mixed-reality ##	No description ##

--winhttp-proxy-resolver ##	Uses WinHTTP to fetch and evaluate PAC scripts. Otherwise the default is to use Chromium's network stack to fetch, and V8 to evaluate. ##

--with-cleanup-mode-logs ##	Identify that the elevated cleaner process is allowed to collect logs. This shouldn't be set if |kExecutionModeSwitch| is not ExecutionMode::kCleaner. ##

--wm-window-animations-disabled ##	If present animations are disabled. ##

--xr\_compositing[1] ##	No description ##

--xsession\_chooser[10] ##	No description ##

--zygote ##	Causes the process to run as a zygote. ##
