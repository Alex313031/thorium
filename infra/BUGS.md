## BUGS.md <img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/bug.svg" width="28">

<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/robot_256.png">

## Document listing known bugs in Thorium or Chromium that will affect usability. Fixed bugs will have a FIXED tag with the date it was fixed at the end. Discoverers/Fixers will have their name at the end.
**2021**

WONTFIX | Nov. 21, Loading PDFs crashes the browser, will be disabling Ink for now to see if it resolves the issue. This has been a bug for a couple of versions now.  EDIT: Bug posted by me here > https://bugs.chromium.org/p/chromium/issues/detail?id=1296914&q=1296914&can=2 - Me | Ink will only work on CrOS for the foreseeable future. May. 28, 2022

BUG | Nov. 21, Pulseaudio does not work behind a proxy in a chroot, I.E. in crouton on ChromiumOS. Removing Pulseaudio patch did not resolve the issue. - Pierro78 on Reddit

UNFIXABLE | Nov. 30, Possible bug; setting the args.gn flag 'enable_cros_media_app = true' does not enable chrome://media-app like CrOS. - Me

FIXED | Nov. 30, Bug that shows starting with M98.0.4738.0; bookmark folders in bookmarks bar have text colored incorrectly. - Me | Fixed upstream on Dec. 2.

FIXED | Dec. 2, The Google API Keys InfoBar patch doesn't work following an upstream update to startup_browser_creator_impl.cc, will evaluate a fix. - Me | Fixed by me on Dec. 5.

FIXED | Dec. 6, The Default Browser InfoBar persists even after setting Thorium as the default browser. Until this is fixed upstream, it will be mitigated with the --no-default-browser-check cmdline flag in the .desktop file. - midzer on GitHub | Fixed by me May. 2022

**2022**

FIXED | Jan. 2, Issue with using AVX builds in windows with the abseil library appeared for many weeks, disappeared, then reappeared, with a final fix being provided by RobRich999. - RobRich999 on GitHub | Fixed by RobRich999

FIXED | Jan. ?, Fix for memcpy in mini_installer that was plaquing me for some time. - RobRich999 on GitHub | Fixed by RobRich999

FIXED | Since M100, Thorium on Windows had a bug where, when running, the icon on the taskbar would be a yellow exclamation icon rather than the proper Thorium icon. -Me | Fixed by me on Feb. 28 2022.

FIXED | Since M98, Thorium would not run correctly on Windows 10/11 without compatiblity mode set to Win 7/8. It has been resolved. - Multiple Users | Fixed by me May. 2022

FIXED | Searching the chrome://settings page on all Windows builds of Thorium causes a crash. Should investigate with bisection and/or a debug build with stack trace or attaching a debugger. UPDATE: Clicking "Customize Thorium" on NTP also triggers same crash. - Multiple Users

BUG | Playing H.265/HEVC Content on Linux does not work - lovelytwo on GitHub |
