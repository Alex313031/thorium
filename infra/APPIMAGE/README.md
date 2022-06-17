## Thorium Appimage Generation <img src="https://raw.githubusercontent.com/Alex313031/Thorium/main/logos/STAGING/Appimage_Logo.svg" width="32">

### Instructions
This directory contains files to generate an .appimage of Thorium.

You __must__ place the .deb file generated from running `thordeb.sh` in this directory, and then run `./build_appimage.sh`

When it is done, you will have an appimage in "out", I.E. it should be something like `//thorium/infra/APPIMAGE/out/Thorium_Browser-104.0.5107.0.glibc2.17-x86_64.AppImage`

You can use it standalone, after making it executable with `sudo chmod +x Thorium_Browser-*`, or (*reccomended*) to integrate it with your system you can use [AppImageLauncher](https://github.com/TheAssassin/AppImageLauncher)

### About
&ndash; This infra project uses *pkg2appimage* from here > https://github.com/AppImage/pkg2appimage \
The *Thorium.yml* file was modeled after the official Chromium one from here > https://github.com/AppImage/pkg2appimage/blob/master/recipes/Chromium.yml
