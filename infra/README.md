## Thorium Infrastructure (Infra) <img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/build_light.svg#gh-dark-mode-only"> <img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/build_dark.svg#gh-light-mode-only">

This contains .gn files, scripts, and other artifacts like vdpau-va-driver and flash player and the BUILDER file which contains notes (generally only usable by me, but feel free to ask what the info inside is for).
The args.list file shows all possible build arguments, and the gn_args.list shows all possible build args after the Thorium args have been added, and the win_args.list shows all possible build arguments for Windows builds.
The &#42;.gn files contain what you should use in the args.gn for platforms other than the normal Linux release (which is in the root of this repo.)
 - 69_release_args.gn is for Chromium 60-70 versions.
 - android_args.gn is for Android ARM64. > https://chromium.googlesource.com/chromium/src/+/main/docs/android_build_instructions.md
 - cros_args.gn is for ChromiumOS on Linux. > https://chromium.googlesource.com/chromium/src/+/main/docs/chromeos_build_instructions.md#Chromium-OS-on-Linux-linux_chromeos
 - mac_args.gn is for x64 macs.
 - windows_args.gn is for Windows x64
 The cgpt is an optimized build of cgpt for Linux/ChromiumOS > https://chromium.googlesource.com/chromiumos/platform/vboot_reference/+/refs/heads/main/README

&nbsp;&ndash; The WIN_INSTRUCTIONS.txt and WIN_CROSS_BUILD_INSTRUCTIONS.txt are for how to build Thorium for Windows natively and on Linux, respectively.

&nbsp;&ndash; The thorium_devel_bookmarks.html file contain the bookmarks I use for Thorium development and rebasing the files it uses.

<img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/thorium_infra_256.png">
