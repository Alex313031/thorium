## Windows Cross-Building MSVS Artifacts Python script. <img src="https://raw.githubusercontent.com/Alex313031/Thorium/main/logos/STAGING/Python.svg" width="36">

&nbsp;&nbsp;Until Chromium fixes this upstream, this file needs to be copied into `depot_tools\win_toolchain` on your Windows machine to generate a Cross-Building artifacts archive if you are using MSVS 2022
 - See > https://chromium.googlesource.com/chromium/src.git/+/HEAD/docs/win_cross.md \
 - Bug I filed related to this > https://bugs.chromium.org/p/chromium/issues/detail?id=1317396

For example, I use *(from cmd.exe in C:\src\depot_tools\win_toolchain\)* `python3 package_from_installed.py 2022 -w 10.0.20348.0`
