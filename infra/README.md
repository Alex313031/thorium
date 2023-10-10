## Thorium Infrastructure (Infra) <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_light.svg#gh-dark-mode-only"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_dark.svg#gh-light-mode-only">

&nbsp;&nbsp;This dir contains [*.gn files*](https://gn.googlesource.com/gn/)<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/gn-logo.png" width="32">, scripts, and other artifacts like vdpau-va-driver and flash player and the BUILDER file which contains notes (generally only usable by me, but feel free to ask what the info inside is for). \
&nbsp;&nbsp;The [args.list](https://github.com/Alex313031/thorium/blob/main/infra/args.list) file shows all possible build arguments, and the [gn_args.list](https://github.com/Alex313031/thorium/blob/main/infra/gn_args.list) shows all possible build args after the Thorium args have been added, and the [win_args.list](https://github.com/Alex313031/thorium/blob/main/infra/win_args.list) shows all possible build arguments for Windows builds. \
&nbsp;&nbsp;The &#42;.gn files contain what you should use in the args.gn for platforms other than the normal Linux release (which is [here](https://github.com/Alex313031/thorium/blob/main/args.gn) in the root of this repo.)

 - The [69_release_args.gn](https://github.com/Alex313031/thorium/blob/main/infra/69_release_args.gn) is for Chromium 60-70 versions.
 - The GN files [Here](https://github.com/Alex313031/thorium/blob/main/arm/android) are for Android (x86, x64, arm32, arm64). > https://chromium.googlesource.com/chromium/src/+/main/docs/android_build_instructions.md
 - The [cros_args.gn](https://github.com/Alex313031/thorium/blob/main/other/CrOS/cros_args.gn) is for ChromiumOS on Linux. > https://chromium.googlesource.com/chromium/src/+/main/docs/chromeos_build_instructions.md#Chromium-OS-on-Linux-linux_chromeos
 - The [mac_args.gn](https://github.com/Alex313031/thorium/blob/main/other/Mac/mac_args.gn) is for x64 MacOS. [mac_ARM_args.gn](https://github.com/Alex313031/thorium/blob/main/other/Mac/mac_ARM_args.gn) is for ARM64 M1/M2 MacOS.)
 - The [win_args.gn](https://github.com/Alex313031/thorium/blob/main/win_args.gn) is for Windows x64 \
 - The [cgpt](https://github.com/Alex313031/thorium/blob/main/infra/cgpt) file is a compiler optimized build of cgpt for Linux/ChromiumOS, you can put anywhere in your $PATH. See > https://chromium.googlesource.com/chromiumos/platform/vboot_reference/+/refs/heads/main/README
 - The DEBUG dir is for DEBUGGING, see the [README.md](https://github.com/Alex313031/thorium/tree/main/infra/DEBUG#readme) inside it.

&nbsp;&ndash; See the [Docs](https://github.com/Alex313031/thorium/tree/main/docs#readme) dir for more instructions on building for a particular platform. \
&nbsp;&ndash; The [DEV_CMDLINE_FLAGS.txt](https://github.com/Alex313031/thorium/blob/main/infra/DEV_CMDLINE_FLAGS.txt) has usefule command line flags for Debugging, Web Development, and Thorium development. The [CMDLINE_FLAGS_LIST.md](https://github.com/Alex313031/thorium/blob/main/infra/CMDLINE_FLAGS_LIST.md) contains a list of all possible cmdline flags for Chromium. It is borrowed from > https://peter.sh/experiments/chromium-command-line-switches/ \
&nbsp;&ndash; The [THORIUM_DEV_BOOKMARKS.html](https://github.com/Alex313031/thorium/blob/main/infra/THORIUM_DEV_BOOKMARKS.html) file contain the bookmarks I use for Thorium development and rebasing the files it uses.

<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/thorium_infra_256.png" width="200">
