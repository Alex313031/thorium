## <img src="https://raw.githubusercontent.com/Alex313031/Thorium/main/logos/NEW/GN_Light.svg#gh-dark-mode-only" width="36"> <img src="https://raw.githubusercontent.com/Alex313031/Thorium/main/logos/NEW/GN_Dark.svg#gh-light-mode-only" width="36"> &nbsp;__When building Chromium/Thorium, you run gn args out/$DIR, where $DIR can be any name; in Thorium it is named out/thorium. These set build arguments for ninja (the Chromium build system) to parse and apply. Thorium has many set, and they vary from platform to platform, and whether or not it is a Debug build. This document explains what they do, why they are set a certain way in Thorium, and options available.__

## __API Keys__

__google_api_key__ "" &#35; Set Google API Key. Unset in public Thorium repository.

__google_default_client_id__ "" &#35; Set the Client ID. Unset in public Thorium repository.

__google_default_client_secret__ "" &#35; Set the Client Secret. All three must be set to use Google Sync, Translate, etc. Unset in public Thorium repository. You can make and set your own by following https://www.chromium.org/developers/how-tos/api-keys/. &#35; NOTE: Thorium contributors, contact me for access to the private API_KEYS repo.

## __Experimental__

__is_chancie_wancie_build__ &#35; Build the CrOS version of ffmpeg. Used for testing, I.E. to enable HEVC on Linux. Will break Windows/Android/Fuchsia builds. Contact me for info on usage cases and testing.

## __MacOS__

__use_system_xcode__ &#35; Whether to use the local XCode. Only applicable on Mac, and must be enabled, as only internal Chrome builds have this set to false to use an internal checkout of XCode.

## __Cross-Building__

__target_os__ &#35; Set the target Operating System. I.E. linux, win, mac, chromeos, fuchsia.

__target_cpu__ &#35; Set the target CPU. I.E. x86, x64, arm, arm64, mipsel, mips64el, and ppc. Can be used to cross-compile. When using arm/arm64, you should set compiler options in //thorium/build/config/arm.gni

__v8_target_cpu__ &#35; What CPU architecture we are building V8 with. Should normally always match target_cpu above.

## __Special__

__use_polly__ &#35; Whether to use Polly optimizations during the linking stage. Disabled by default.

__is_raspi__ &#35; Whether to enable Raspberry Pi specific optimizations. Only valid for arm64 targets.

__is_thorium__ &#35; Whether this is a Thorium build or Chromium build. Set to true in Thorium. When set to false, it does not build the custom top bar icons and other icons used throughout the UI. The main logo is unaffected by this arg.

## __Main__

__enable_linux_installer__ &#35; Enable making a linux .deb, .rpm, or .snap installer. (Linux Only) Disabled in Debug builds.

__enable_distro_version_check__ &#35; Set whether to make the linux installer check for supported distro. (Linux Only) Disabled in Debug builds.

__enable_stripping__ &#35; Set whether to strip binaries and .so/.dll libraries. Disabled in Debug builds.

__is_official_build__ &#35; Set whether this is an Official Build. This enables other optimizations and sets "Official" instead of "Developer" in chrome://version and chrome://settings/help Disabled in Debug builds, and always enabled for public releases.

__is_debug__ &#35; Set whether to link debugging constructs into the binary. Enabled in Debug builds. See > https://chromium.googlesource.com/chromium/src/+/0e94f26e8/docs/linux_debugging.md and https://www.chromium.org/developers/how-tos/debugging-on-windows/

__dcheck_always_on__ &#35; Set whether to enable DCHECK, regardless of is_debug or is_official_build. Disabled in Thorium for perforance/size except for Debug builds.

exclude_unwind_tables &#35; Set whether to enable unwinding tables, regardless of is_debug or is_official_build. Enabled in Thorium for performance/size except for Debug builds.

__enable_iterator_debugging__ &#35; Set whether C++ iterator debugging is enabled. Only applicable if is_debug is true.

__disable_fieldtrial_testing_config__ &#35; Set whether to disable fieldtrials. Disabled for stability, privacy, and reproducibility in Thorium. Decreases the number of variations at chrome://version See > https://chromium.googlesource.com/chromium/src/+/master/testing/variations/README.md

__enable_resource_allowlist_generation__ &#35; Enables allowlist generation for IDR_ grit defines seen by the compiler. Set to false in Thorium, except for Windows and Android builds.

__enable_profiling__ &#35; Set whether to enable profiling, I.E. for making your own PGO *.profdata files. Set to false in Thorium for performance. See > https://chromium.googlesource.com/chromium/src.git/+/HEAD/docs/profiling.md Should be disabled in public ThoriumOS builds.

__is_component_build__ &#35; Splits many parts of Thorium/Chromium into shared libraries. Avoids the long linking step towards the end of building. In Debug builds, this is what disables the creation of an installer. See > https://chromium.googlesource.com/chromium/src/+/HEAD/docs/component_build.md Set to disabled in Thorium, except for Debug builds, where it is the default.

__symbol_level__ &#35; Sets overall symbol level. Options are: 0, 1, and 2. Set to 0 for performance.

__enable_nacl__ &#35; Set whether Native Client (NaCl) is built. Disabled in Thorium for size, and because it is being deprecated. Enabled for ChromiumOS/ThoriumOS builds for Chrome Apps support. See > https://developer.chrome.com/docs/native-client/

__optimize_webui__ &#35; Set whether WebUI components are optimized. Uses polymer-bundler to combine/compress HTML5 resources. See > https://chromium.googlesource.com/chromium/src/+/HEAD/docs/optimizing_web_uis.md Enabled in Thorium for performance.

__enable_webui_tab_strip__ &#35; Set whether the Tab Strip, and some parts of the top bar (Top Chrome) are built using WebUI technologies. Should always be enabled, except on certain old linux distros.

__use_lld__ &#35; Use LLVM lld instead of GNU ld for linking. Should always be enabled unless you are using another linker like MOLD or building with GCC.

__use_icf__ &#35; Whether to use Identical Code Folding, which mergess identical code constructs from different source files into one machine code object. Enabled by default for performance and binary size.

__is_cfi__ &#35; Whether to enable Control Flow Integrity. Should always be enabled for security on Linux based platforms. The equivalent for Windows is win_enable_cfg_guards, noted below.

__win_enable_cfg_guards__ &#35; Whether to enable Control Flow Integrity on Windows. Should always be enabled for security.

__v8_symbol_level__ &#35; Set the symbol level for V8 (Chromium's JavaScript engine), regardless of symbol_level value. Options are: 0, 1, and 2. Set to 0 for performance. For more info about V8, see > https://v8.dev/

__v8_enable_fast_torque__ &#35; Always compile to Torque component of V8 in non-debug mode. Enabled by default in Thorium and in official builds, but can optionally be disabled.

__v8_enable_builtins_optimization__ &#35; Whether to enable PGO for V8. Set to true in Thorium for performance, even on Windows and MacOS, where it is disabled by default upstream because of long build times.

__v8_enable_maglev__ &#35; Enable the new Maglev JIT optimizing JS compiler component of V8. Enabled in Thorium for performance, except on 32-bit targets, where it is not applicable.

__v8_enable_turbofan__ &#35;  Enable the TurboFan optimizing JS compiler component of V8. Enabled in Thorium for performance, except on 32-bit targets. TODO: Possibly re-enable on 32-bit targets?

__v8_enable_wasm_simd256_revec__ &#35; Whether WebAssembly is compiled with 256-bit long SIMD code. Set to true in Thorium for performance.

__use_v8_context_snapshot__ &#35; Build a separate .bin file with V8's function templates and V8 contexts. Enabled in Thorium.

__blink_symbol_level__ &#35; Set the symbol level for Blink (Chromium's rendering engine.), regardless of symbol_level value. Options are: 0, 1, and 2. Set to 0 for performance.

__enable_precompiled_headers__ &#35; Sets whether to precompile some .h files before main compilation. Can speed up compiling, but is disabled in Thorium for reproducibility and to ensure any Thorium .h files are properly #included in corresponding .cc files. See > https://cmake.org/cmake/help/latest/command/target_precompile_headers.html

__media_use_ffmpeg__ &#35; Enable use of Chromium's ffmpeg implementation for media library/streams. Should always be enabled unless using a custom ffmpeg checkout.

__media_use_libvpx__ &#35; Enable use of Chromium's libvpx implementation for media library/streams. Used for software based decoding of VP9 and VP8A type content. Should always be enabled.

__enable_hls_demuxer__ &#35; Enables HLS media demuxing. &#35; INCOMPLETE upstream. Enabled in Thorium for useability.

__enable_ink__ &#35; Enable experimental Ink (PDF Annotation) on CrOS. Breaks non-ChromiumOS builds. &#35; NOTE: Bug filed by me upstream.

__enable_discovery__ &#35; Enable the Discovery SWA (System Web App) on CrOS. Has no effect on non-ChromiumOS builds. Only fully works on internal ChromeOS builds.

__enable_cros_media_app__ &#35; Enable the CrOS Media App, a native app for viewing media on ChromiumOS, to replace the native Files App views component. Breaks non-ChromiumOS builds. Should be enabled in ThoriumOS. &#35; NOTE: Bug filed by me upstream.

__proprietary_codecs__ &#35; Enable building ffmpeg with proprietary codecs like H.254, H.265 (HEVC), and FLAC. Must be used with ffmpeg_branding = chrome or ffmpeg_branding = chromeos. Enabled in Thorium for useability. See > https://www.chromium.org/audio-video/

__ffmpeg_branding__ &#35; Sets the internal BRANDING header in ffmpeg. Must be used with proprietary_codecs = true. Options are: "Chromium", "Chrome", and "Chromeos". is_chancie_wancie = true build will override the internal checks that "Chromeos" only be set when building for CrOS, and set it to Chromeos anyway for any platform. 

__clang_use_chrome_plugins__ &#35; Whether to use Chromium specific plugins in the clang compiler to check for type errors. Set to true normally, but must be set to false for ThoriumOS, or when using a custom clang compiler (such as when using Polly)

__enable_ffmpeg_video_decoders__ &#35; Set whether software ffmpeg video decoders are enabled. Should be set to true. Used in Thorium as a fallback for all platforms when a hardware decoder, I.E. VAAPI or VDPAU cannot be used.

__is_component_ffmpeg__ &#35; Builds ffmpeg as a shared library even when is_component_build is false or is_official build is true. Enabled on Linux, so users can swap out libffmpeg.so for a FOSS one or different one, I.E a different version or from something like Opera or Vivaldi for extra Codec support. &#35; NOTE: Should be disabled on Mac, because building the installer will fail.

__use_webaudio_pffft__ &#35; Use the pffft library for computing FFT's in audio streams. Enabled in Thorium for performance EXCEPT on MacOS, because the native FFT library there is much faster. See > https://bitbucket.org/jpommier/pffft/src

__use_vaapi__ &#35; Set the availability of VAAPI in Chromium. Should always be enabled in Thorium, except on Windows.

__enable_widevine__ &#35; Set the availability of the Widevine CDM (Content Decryption Module) for encrypted media streams like I.E on Netflix, Hulu, etc. Enabled in Thorium for useability. See > https://www.widevine.com/

__bundle_widevine_cdm__ &#35; Set whether to bundle the Widevine CDM as a library in the installation folder. When disabled, it functions as a component and is downloaded with other components from Google servers to the user profile dir.

__ignore_missing_widevine_signing_cert__ &#35; Set whether to ignore Widevine CDM host verification signing due to no signing cert, regardless of is_official_build. Enabled in Thorium to mitigate possible Widevine issues, but makes VMP not work.

__enable_media_drm_storage__ &#35; Enable whether Widevine can use LocalStorage to hold DRM files for certain encrypted streams, like I.E. Amazon Prime. Enabled in Thorium for useability.

__enable_media_overlay__ &#35; Set whether certain media overlays, I.E. volume bar, etc. are enabled. Enabled in Thorium for useability.

__enable_hangout_services_extension__ &#35; Enables building the Hangouts component extension for Chromium/ChromiumOS. Enabled in Thorium for feature parity with Chrome. Should be enabled on ThoriumOS.

__rtc_use_h264__ &#35; Enable WebRTC (Real-Time Communication) to use H.264 for transmission/demuxing. Must be used with proprietary_codecs = true and ffmpeg_branding set to "Chrome" or "Chromeos". Enabled in Thorium for useability.

__rtc_build_with_neon__ &#35; Set whether to use ARM NEON SIMD instructions when building RTC code. Only applicable for ARM/ARM64. Enabled in Thorium for performance.

__rtc_build_examples__ &#35; Set whether to build example RTC data. Useful for debugging. Disabled in Thorium for size.

__rtc_enable_avx2__ &#35; Set whether to use AVX2 instructions when building RTC code. Enabled only in Thorium AVX/AVX2 builds for performance.

__enable_vr__ &#35; Set whether to enable WebVR and WebXR in Chromium. Enabled in Thorium for useability. See > https://webvr.info/ and https://immersiveweb.dev/

__enable_platform_hevc__ &#35; Set the availability of using hardware HEVC decoders/encoders. Also needed to enable H.265. Enabled in Thorium on all platforms, despite H.264 being unavailable right now for Linux. Should be enabled in ThoriumOS.

__enable_hevc_parser_and_hw_decoder__ &#35; Enable parsing and demuxing of HEVC media. Enabled in Thorium for useability. See > https://en.wikipedia.org/wiki/High_Efficiency_Video_Coding

__enable_platform_ac3_eac3_audio__ &#35; Enable building ac3 and eac3 audio codecs in ffmpeg. Enabled in Thorium along with FFMPEG patches to enable these codecs.

__enable_platform_dolby_vision__ &#35; Enable Dolby vision in Chromium. Enabled in Thorium for all platforms, despite only working fully on ChromiumOS and ChromeCast. Should be enabled in ThoriumOS.

__enable_platform_encrypted_dolby_vision__ &#35; Same as above, but for Widevine encrypted streams. Enabled in Thorium/ThoriumOS, for same reasons as above.

__enable_platform_mpeg_h_audio__ &#35; Enable experimental MPEG H audio. Enabled in Thorium for useability.

__enable_platform_dts_audio__ &#35; Enable experimental MPEG H audio. Enabled in Thorium for useability.

__enable_mse_mpeg2ts_stream_parser__ &#35; Enable experimental MPEG-TS media stream parsing/demuxing. Doesn't seem to work on non CrOS platforms. Enabled anyway in Thorium for all platforms for useability.

__enable_rust__ &#35; Whether to enable the Rust toolchain during compilation. Set to true for all platforms except MacOS.

## __Optimizations__

__use_text_section_splitting__ &#35; Whether to split text sections into their own section in the binary, which can increase performance when using thinLTO and PGO together. Set to true in Thorium.

__use_thin_lto__ &#35; Enable thinLTO optimizations. Should be enabled for all Thorium builds, even Debug ones. See > https://clang.llvm.org/docs/ThinLTO.html

__thin_lto_enable_optimizations__ &#35; Enable more aggressive thinLTO optimizations. Enabled in Thorium for performance, and makes use of the -O3 Cflags, LDflags, and import_instr_limit = 30 flags in the main chrome BUILD.gn file. Disabled for Debug builds.

__init_stack_vars_zero__ &#35; Not entirely sure what this does, but enabled at the suggestion of RobRich999.

__chrome_pgo_phase__ &#35; Set the implementation of PGO in Chromium. Options are: 0 (none), 1 (Instrumentation Phase Only), and 2 (Full PGO). 1 and 2 requires setting a *.profdata file for the platform below. Thorium uses 2, except for Debug builds. 1 should only be used when profiling. Cannot be enabled on ChromiumOS/ThoriumOS. Can be disabled for faster compilation. Disabled for Debug builds. See > https://en.wikipedia.org/wiki/Profile-guided_optimization

__pgo_data_path__ &#35; Set the full path to the *.profdata file for PGO. Downloaded when running trunk.sh in Thorium. Disabled in Debug builds and ThoriumOS.
