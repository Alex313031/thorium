## <img src="https://raw.githubusercontent.com/Alex313031/Thorium/main/logos/NEW/GN_Light.svg#gh-dark-mode-only" width="36"> <img src="https://raw.githubusercontent.com/Alex313031/Thorium/main/logos/NEW/GN_Dark.svg#gh-light-mode-only" width="36"> &nbsp;When building Chromium/Thorium, you run gn args out/$DIR, where $DIR can be any name; in Thorium it is named out/thorium. These set build arguments for ninja (the Chromium build system) to parse and apply. Thorium has many set, and they vary from platform to platform, and whether or not it is a Debug build. This document explains what they do, why they are set a certain way in Thorium, and options available.

## API Keys
google_api_key = "" &#35;&#35; Set Google API Key. Unset in public Thorium repository.

google_default_client_id = "" &#35;&#35; Set the Client ID. Unset in public Thorium repository.

google_default_client_secret = "" &#35;&#35; Set the Client Secret. All three must be set to use Google Sync, Translate, etc. Unset in public Thorium repository. You can make and set your own by following https://www.chromium.org/developers/how-tos/api-keys/. &#35;&#35; NOTE: Thorium contributers, contact me for access to the private API_KEYS repo.

## Experimental
is_chancie_wancie_build &#35;&#35; Build the CrOS version of ffmpeg. Used for testing, I.E. to enable HEVC on Linux. Will break Windows/Android/Fuchsia builds. Contact me for info on usage cases and testing.

## MacOS
use_system_xcode = true &#35;&#35; Whether to use the local XCode. Only applicable on Mac, and must be enabled, as only internal Chrome builds have this set to false to use an internal checkout of XCode.

## Cross-Building
target_os &#35;&#35; Set the target Operating System. I.E. linux, win, mac, chromeos, fuchsia.

target_cpu &#35;&#35; Set the target CPU. I.E. x86, x64, arm, arm64, mipsel, mips64el, and ppc. Can be used to cross-compile. When using arm/arm64, you should set compiler options in //thorium/build/config/arm.gni

## Main
enable_linux_installer &#35;&#35; Enable making a linux .deb, .rpm, or .snap installer. (Linux Only) Disabled in Debug builds.

enable_distro_version_check &#35;&#35; Set whether to make the linux installer check for supported distro. (Linux Only) Disabled in Debug builds.

enable_stripping &#35;&#35; Set whether to strip binaries and .so/.dll libraries. Disabled in Debug builds.

is_official_build &#35;&#35; Set whether this is an Official Build. This enables other optimizations and sets "Official" instead of "Developer" in chrome://version and chrome://settings/help Disabled in Debug builds.

is_debug &#35;&#35; Set whether to link debugging constructs into the binary. Enabled in Debug builds. See > https://chromium.googlesource.com/chromium/src/+/0e94f26e8/docs/linux_debugging.md and https://www.chromium.org/developers/how-tos/debugging-on-windows/

dcheck_always_on &#35;&#35; Set whether to enable DCHECK, regardless of is_debug or is_official_build. Disabled in Thorium for perforance/size except for Debug builds.

exclude_unwind_tables &#35;&#35; Set whether to enable unwinding tables, regardless of is_debug or is_official_build. Enabled in Thorium for performance/size except for Debug builds.

enable_debugallocation &#35;&#35; DEPRECATED &#35;&#35; Set whether to allocate extra header space in binaries for debug constructs.

enable_iterator_debugging &#35;&#35; Set whether C++ iterator debugging is enabled. Only applicable if is_debug is true.

disable_fieldtrial_testing_config &#35;&#35; Set whether to disable fieldtrials. Disabled for stability, privacy, and reproducibility in Thorium. Decreases the number of variations at chrome://version See > https://chromium.googlesource.com/chromium/src/+/master/testing/variations/README.md

enable_resource_allowlist_generation &#35;&#35; Enables allowlist generation for IDR_ grit defines seen by the compiler. Only applicable for Android and Windows builds. Set to false in Thorium.

enable_profiling &#35;&#35; Set whether to enable profiling, I.E. for making your own PGO *.profdata files. Set to false in Thorium for performance. See > https://chromium.googlesource.com/chromium/src.git/+/HEAD/docs/profiling.md Should be disabled in public ThoriumOS builds.

is_component_build &#35;&#35; Splits many parts of Thorium/Chromium into shared libraries. Avoids the long linking step towards the end of building. In Debug builds, this is what disables the creation of an installer. See > https://chromium.googlesource.com/chromium/src/+/HEAD/docs/component_build.md Set to disabled in Thorium, even for Debug builds, where it is the default.

symbol_level &#35;&#35; Sets overall symbol level. Options are: 0, 1, and 2. Set to 0 for performance.

enable_nacl &#35;&#35; Set whether Native Client (NaCl) is built. Disabled in Thorium for size, and because it is being deprecated. Enable for using some Chrome Apps and for ChromiumOS/ThoriumOS builds. See > https://developer.chrome.com/docs/native-client/

optimize_webui &#35;&#35; Set whether WebUI components are optimized. Uses polymer-bundler to combine/compress HTML5 resources. See > https://chromium.googlesource.com/chromium/src/+/HEAD/docs/optimizing_web_uis.md Enabled in Thorium for performance.

enable_webui_tab_strip &#35;&#35; Set whether the Tab Strip, and some parts of the top bar (Top Chrome) are built using WebUI technologies. Should always be enabled, except on certain old linux distros.

use_lld &#35;&#35; Use LLVM lld instead of GNU ld for linking. Should always be enabled unless you are using another linker like MOLD.

v8_symbol_level &#35;&#35; Set the symbol level for V8 (Chromium's JavaScript engine), regardless of symbol_level value. Options are: 0, 1, and 2. Set to 0 for performance. For more info about V8, see > https://v8.dev/

use_v8_context_snapshot &#35;&#35; Build a seperate .bin file with V8's function templates and V8 contexts. Enabled in Thorium. &#35;&#35; TODO: Maybe disable this on Linux builds? Should be disabled in ThoriumOS.

blink_symbol_level &#35;&#35; Set the symbol level for Blink (Chromium's rendering engine.), regardless of symbol_level value. Options are: 0, 1, and 2. Set to 0 for performance.

enable_precompiled_headers &#35;&#35; Sets whether to precompile some .h files before main compiling. Can speed up compiling, but is disabled in Thorium for reproducibility and to ensure any Thorium .h files are properly #included in corresponding .cc files. See > https://cmake.org/cmake/help/latest/command/target_precompile_headers.html

media_use_ffmpeg &#35;&#35; Enable use of Chromium's ffmpeg implementation for media library/streams. Should always be enabled unless using a custom ffmpeg checkout.

media_use_libvpx &#35;&#35; Enable use of Chromium's libvpx implementation for media library/streams. Used for software based decoding of VP9 and VP8A type content. Should always be enabled.

enable_hls_sample_aes &#35;&#35; Enables demuxing of HLS media encrypted with AES. Uses the AES Cflags in Thorium to increase performance. See > https://en.wikipedia.org/wiki/AES_instruction_set

enable_hls_demuxer &#35;&#35; Enables HLS media demuxing. &#35;&#35; INCOMPLETE upstream. Enabled in Thorium for useability.

enable_ink &#35;&#35; Enable experimental Ink (PDF Annotation) on CrOS. Breaks non-ChromiumOS builds. &#35;&#35; NOTE: Bug filed by me upstream.

enable_discovery &#35;&#35; Enable the Discovery SWA (System Web App) on CrOS. Has no effect on non-ChromiumOS builds. Only fully works on internal ChromeOS builds.

enable_cros_media_app &#35;&#35; Enable the CrOS Media App, a native app for viewing media on ChromiumOS, to replace the native Files App views component. Breaks non-ChromiumOS builds. Should be enabled in ThoriumOS. &#35;&#35; NOTE: Bug filed by me upstream.

proprietary_codecs &#35;&#35; Enable building ffmpeg with proprietary codecs like H.254, H.265 (HEVC), and FLAC. Must be used with ffmpeg_branding = chrome or ffmpeg_branding = chromeos. Enabled in Thorium for useability. See > https://www.chromium.org/audio-video/

ffmpeg_branding &#35;&#35; Sets the internal BRANDING header in ffmpeg. Must be used with proprietary_codecs = true. Options are: "Chromium", "Chrome", and "Chromeos". is_chancie_wancie = true build will override the internal checks that "Chromeos" only be set when building for CrOS, and set it to Chromeos anyway for any platform. 

enable_ffmpeg_video_decoders &#35;&#35; Set whether software ffmpeg video decoders are enabled. Should be set to true. Used in Thorium as a fallback for all platforms when a hardware decoder, I.E. VAAPI or VDPAU cannot be used.

is_component_ffmpeg &#35;&#35; Builds ffmpeg as a shared library even when is_component_build is false or is_official build is true. Enabled on Linux, so users can swap out libffmpeg.so for a FOSS one or different one, I.E a different version or from something like Opera or Vivaldi for extra Codec support. &#35;&#35; NOTE: Should be disabled on Mac, because building the installer will fail. Also, should be disabled on Windows, as it causes the compatibility mode bug on Win 10/11. &#35;&#35; TODO: Look into fixing on Windows, so that users can swap out ffmpeg.dll for another one.

use_webaudio_ffmpeg &#35;&#35; Use ffmpeg for computing FFT's (Fast Fourier Transform) in audio streams. Disabled in Thorium for performance, and because of bugs when building for AVX.

use_webaudio_pffft &#35;&#35; Use the pffft library for computing FFT's in audio streams. Enabled in Thorium for performance EXCEPT on MacOS, because the native FFT library there is much faster. See > https://bitbucket.org/jpommier/pffft/src

enable_av1_decoder &#35;&#35; Enable standard AV1 decoder. Commented out in Thorium to let ninja set appropriate value for the platform.

enable_dav1d_decoder &#35;&#35; Enable VideoLAN dav1d AV1 decoder. Commented out in Thorium to let ninja set appropriate value for the platform.

use_vaapi &#35;&#35; Set the availability of VAAPI in Chromium. Should always be enabled in Thorium, except on Windows.

enable_widevine &#35;&#35; Set the availability of the Widevine CDM (Content Decryption Module) for encrypted media streams like I.E on Netflix, Hulu, etc. Enabled in Thorium for useability. See > https://www.widevine.com/

bundle_widevine_cdm &#35;&#35; Set whether to bundle the Widevine CDM as a library in the installation folder. When enabled, it cannot be updated as its version is hard coded and tied with the chrome executable, and will not show in chrome://components as it is not built as a component. When disabled, it functions as a component and is downloaded with other components from Google servers to the user profile dir. Disabled in Thorium for transparency, and so that it can be updated irrespective of the Thorium version. Also disabled to make use of the widevine patches for Thorium on Linux. Could technically be enabled on Windows. Only enabled in ThoriumOS, with the widevine patches in that repository.

ignore_missing_widevine_signing_cert &#35;&#35; Set whether to ignore Widevine CDM host verification signing due to no signing cert, regardless of is_official_build. Enabled in Thorium to mitigate possible Widevine issues.

enable_media_drm_storage &#35;&#35; Enable whether Widevine can use LocalStorage to hold DRM files for certain encrypted streams, like I.E. Amazon Prime. Enabled in Thorium for useability.

enable_media_overlay &#35;&#35; Set whether certain media overlays, I.E. volume bar, etc. are enabled. Enabled in Thorium for useability.

enable_hangout_services_extension &#35;&#35; Enables building the Hangouts component Extension on Chromium/ChromiumOS. Enabled in Thorium for feature parity with Chrome. Should be enabled on ThoriumOS.

rtc_use_h264 &#35;&#35; Enable WebRTC (Real-Time Communication) to use H.264 for transmission/demuxing. Must be used with proprietary_codecs = true and ffmpeg_branding set to "Chrome" or "Chromeos". Enabled in Thorium for useability.

rtc_use_lto &#35;&#35; Set whether WebRTC uses thinLTO for optimization. Enabled in Thorium for performance.

rtc_build_with_neon &#35;&#35; Set whether to use ARM NEON SIMD instructions when building RTC code. Only applicable for ARM/ARM64. Enabled in Thorium for performance.

rtc_build_examples &#35;&#35; Set whether to build example RTC data. Useful for debugging. Disabled in Thorium for size.

rtc_enable_avx2 &#35;&#35; Set whether to use AVX2 instructions when building RTC code. Enabled only in Thorium AVX2 builds for performance.

enable_vr &#35;&#35; Set whether to enable WebVR and WebXR in Chromium. Enabled in Thorium for useability. See > https://webvr.info/ and https://immersiveweb.dev/

use_vr_assets_component &#35;&#35; Set whether to enable the VR assets component. Disabled by default in non-Chrome builds. Enabled in Thorium for useability. Should also be enabled in ThoriumOS.

enable_platform_hevc &#35;&#35; Set the availability of using hardware HEVC decoders/encoders. Also needed to enable H.265. Enabled in Thorium on all platforms, despite H.264 being unavailable right now for Linux. Should be enabled in ThoriumOS.

enable_hevc_parser_and_hw_decoder &#35;&#35; Enable parsing and demuxing of HEVC media. Enabled in Thorium for useability. See > https://en.wikipedia.org/wiki/High_Efficiency_Video_Coding

enable_platform_ac3_eac3_audio &#35;&#35; Enable building ac3 and eac3 audio codecs in ffmpeg. Enabled in Thorium for useability, despite being unavailable for some platforms. &#35;&#35; TODO: What platforms?

enable_platform_dolby_vision &#35;&#35; Enable Dolby vision in Chromium. Enabled in Thorium for all platforms, despite only working fully on ChromiumOS and ChromeCast. Should be enabled in ThoriumOS.

enable_platform_mpeg_h_audio &#35;&#35; Enable experimental MPEG H audio. Enabled in Thorium for useability.

enable_mse_mpeg2ts_stream_parser &#35;&#35; Enable experimental MPEG-TS media stream parsing/demuxing. Doesn't seem to work on non CrOS platforms. Enabled anyway in Thorium for all platforms for useability.

## Optimizations
use_thin_lto &#35;&#35; Enable thinLTO optimizations. Should be enabled for all Thorium builds, even Debug ones. See > https://clang.llvm.org/docs/ThinLTO.html

thin_lto_enable_optimizations &#35;&#35; Enable more aggressive thinLTO optimizations. Enabled in Thorium for performance, and makes use of the -O3 Cflags, LDflags, and import_instr_limit = 30 flags in the main chrome BUILD.gn file. Disabled for Debug builds.

chrome_pgo_phase &#35;&#35; Set the implementation of PGO in Chromium. Options are: 0 (none), 1 (Instrumentation Phase Only), and 2 (Full PGO). 1 and 2 requires setting a *.profdata file for the platform below. Thorium uses 2, except for Debug builds. 1 should only be used when profiling. Cannot be enabled on ChromiumOS/ThoriumOS. Can be disabled for faster compilation. Disabled for Debug builds. See > https://en.wikipedia.org/wiki/Profile-guided_optimization

pgo_data_path &#35;&#35; Set the full path to the *.profdata file for PGO. Downloaded when running trunk.sh in Thorium. Disabled in Debug builds and ThoriumOS.
