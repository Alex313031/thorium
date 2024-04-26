## Thorium Debugging Infra <img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/bug.svg" width="28">

 - This contains [*.gn files*](https://gn.googlesource.com/gn/) and scripts for generating DEBUG builds of Thorium for debugging, testing, and inspection.
 - The [ABOUT_GN_ARGS.md](https://github.com/Alex313031/thorium/blob/main/infra/DEBUG/ABOUT_GN_ARGS.md) describes what each line in the args &#42;.gn files do, also useful for the regular build args &#42;.gn files. \
&nbsp;&nbsp; __NOTE:__ You cannot build installers for any platform with a debug build. Running the [clean.sh](https://github.com/Alex313031/thorium/blob/main/clean.sh) script in the root of the repo is highly recommended to get your //out/thorium dir from ~6-7 GB to something reasonable, like ~1-2 GB.
 - Running the `build_debug.sh` or `build_win_debug.sh` will build the *Thorium UI Debug Shell (views_examples_with_content)* target as well as the rest of Thorium.
 - Running `build_debug_shell.sh` can be used for any platform, and will build the standalone Thorium UI Debug Shell. \
&nbsp;&nbsp; __NOTE:__ Running either of the above will make a directory in //out/thorium called Thorium_UI_Debug_Shell with all the artifacts and Readme inside, suitable for zipping up and distributing to users.
 - For more information, read the [DEBUG_SHELL_README.md](https://github.com/Alex313031/thorium/blob/main/infra/DEBUG/DEBUG_SHELL_README.md) file).
 
### More Info <a name="moreinfo"></a>
__The [*DEBUGGING.md*](https://github.com/Alex313031/thorium/blob/main/infra/DEBUG/DEBUGGING.md) file is a slightly modified version of the upstream Chromium one.__ \
__*&#42;For more information about debugging,* See > [Logging](https://www.chromium.org/for-testers/enable-logging/), &nbsp;[Network Logging](https://www.chromium.org/for-testers/providing-network-details/), &nbsp;[Linux](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/linux/debugging.md), &nbsp;[MacOS](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/mac/debugging.md), &nbsp;[Android](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/android_debugging_instructions.md), &nbsp;[ChromiumOS](https://www.chromium.org/chromium-os/how-tos-and-troubleshooting/debugging-features/), &nbsp;and [Windows](https://chromium.googlesource.com/playground/chromium-org-site/+/refs/heads/main/developers/how-tos/debugging-on-windows/index.md), &nbsp;including [WinDBG Help](https://chromium.googlesource.com/playground/chromium-org-site/+/refs/heads/main/developers/how-tos/debugging-on-windows/windbg-help.md), &nbsp;and [Dump Example](https://chromium.googlesource.com/playground/chromium-org-site/+/refs/heads/main/developers/how-tos/debugging-on-windows/example-of-working-with-a-dump.md).__

<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/thorium_infra_256.png" width="200">
