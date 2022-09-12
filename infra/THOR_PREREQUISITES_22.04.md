### Dependencies to build Thorium on Ubuntu 22.04 "Jammy Jellyfish", since *//chromium/src/build/install-build-deps.sh* has not yet been updated.

Step 1. Add `umask 022` to your __*.bashrc*__ file ahead of any export lines.

Step 2. Modify __*/etc/sysctl.conf*__ and add a line that says `fs.file-max=16384`

Step 3. Install the packages that would have been installed by *install-build-deps.sh*

`
sudo apt install at-spi2-core binutils binutils-arm-linux-gnueabihf binutils-arm-linux-gnueabihf binutils-doc binutils-mips64el-linux-gnuabi64 binutils-mipsel-linux-gnu bison bzip2 cdbs curl dbus-x11 devscripts dpkg-dev elfutils fakeroot flex g++-10-arm-linux-gnueabi g++-10-multilib g++-arm-linux-gnueabihf gcc-arm-linux-gnueabi gcc-arm-linux-gnueabihf git-core gperf icoutils libapt-pkg6.0 libasound2 libasound2-dev libasound2:i386 libatk-bridge2.0-0 libatk-bridge2.0-dev libatk1.0-0 libatk1.0-0:i386 libatspi2.0-0 libatspi2.0-dev libavahi-client3 libbrlapi-dev libbrlapi0.8 libbz2-dev libc6 libc6-dev libc6-dev-armhf-cross libcairo2 libcairo2-dev libcap-dev libcap2 libcups2 libcups2-dev libcurl4-gnutls-dev libdbus-1-3 libdbus-1-dev libdrm-dev libdrm2 libegl1 libegl1:i386 libelf-dev libevdev-dev libevdev2 libexpat1 libffi-dev libfontconfig1 libfreetype6 libfuse-dev libgbm-dev libgbm1 libgl1 libgl1:i386 libglib2.0-0 libglib2.0-0:i386 libglib2.0-dev libglu1-mesa-dev libgtk-3-0 libgtk-3-dev libinput-dev libjpeg62 libkrb5-dev libnspr4-dev libnss3-dev libnss3:i386 libpam0g libpam0g-dev libpango-1.0-0 libpango-1.0-0:i386 libpango1.0-dev libpci-dev libpci3 libpcre3 libpixman-1-0 libpng++-dev libpng-dev libpng16-16 libpulse-dev libpulse-mainloop-glib0 libpulse0 libsctp-dev libspeechd-dev libspeechd2 libsqlite3-0 libsqlite3-dev libssh2-1-dev libssl-dev libstdc++6 libudev-dev libudev0 libudev1 libuuid1 libva-dev libvulkan-dev libwayland-egl1 libwayland-egl1-mesa libwayland-egl1:i386 libwww-perl libx11-6 libx11-xcb1 libx11-xcb1:i386 libxau6 libxcb1 libxcomposite1 libxcomposite1:i386 libxcursor1 libxdamage1 libxdamage1:i386 libxdmcp6 libxext6 libxfixes3 libxi6 libxinerama1 libxkbcommon0:i386 libxrandr2 libxrandr2:i386 libxrender1 libxshmfence-dev libxslt1-dev libxss-dev libxt-dev libxtst-dev libxtst6 libxtst6:i386 linux-libc-dev-armhf-cross locales openbox p7zip patch perl pkg-config rpm rpm-common rpm2cpio rrdtool ruby subversion texinfo uuid-dev wdiff x11-utils xcompmgr xvfb xz-utils zip zlib1g lib32z1 lighttpd libncurses5:i386 libstdc++6:i386 zlib1g:i386 libsquashfuse0 squashfs-tools libfuse3-3 libfuse2 gvfs-fuse fuse3 exfat-fuse
`

---------------------------------------------------------------
Easy Copy/Paste:

```shell
sudo apt install at-spi2-core binutils binutils-arm-linux-gnueabihf binutils-arm-linux-gnueabihf binutils-doc binutils-mips64el-linux-gnuabi64 binutils-mipsel-linux-gnu bison bzip2 cdbs curl dbus-x11 devscripts dpkg-dev elfutils fakeroot flex g++-10-arm-linux-gnueabi g++-10-multilib g++-arm-linux-gnueabihf gcc-arm-linux-gnueabi gcc-arm-linux-gnueabihf git-core gperf icoutils libapt-pkg6.0 libasound2 libasound2-dev libasound2:i386 libatk-bridge2.0-0 libatk-bridge2.0-dev libatk1.0-0 libatk1.0-0:i386 libatspi2.0-0 libatspi2.0-dev libavahi-client3 libbrlapi-dev libbrlapi0.8 libbz2-dev libc6 libc6-dev libc6-dev-armhf-cross libcairo2 libcairo2-dev libcap-dev libcap2 libcups2 libcups2-dev libcurl4-gnutls-dev libdbus-1-3 libdbus-1-dev libdrm-dev libdrm2 libegl1 libegl1:i386 libelf-dev libevdev-dev libevdev2 libexpat1 libffi-dev libfontconfig1 libfreetype6 libgbm-dev libgbm1 libgl1 libgl1:i386 libglib2.0-0 libglib2.0-0:i386 libglib2.0-dev libglu1-mesa-dev libgtk-3-0 libgtk-3-dev libinput-dev libjpeg62 libkrb5-dev libnspr4-dev libnss3-dev libnss3:i386 libpam0g libpam0g-dev libpango-1.0-0 libpango-1.0-0:i386 libpango1.0-dev libpci-dev libpci3 libpcre3 libpixman-1-0 libpng++-dev libpng-dev libpng16-16 libpulse-dev libpulse-mainloop-glib0 libpulse0 libsctp-dev libspeechd-dev libspeechd2 libsqlite3-0 libsqlite3-dev libssh2-1-dev libssl-dev libstdc++6 libudev-dev libudev0 libudev1 libuuid1 libva-dev libvulkan-dev libwayland-egl1 libwayland-egl1-mesa libwayland-egl1:i386 libwww-perl libx11-6 libx11-xcb1 libx11-xcb1:i386 libxau6 libxcb1 libxcomposite1 libxcomposite1:i386 libxcursor1 libxdamage1 libxdamage1:i386 libxdmcp6 libxext6 libxfixes3 libxi6 libxinerama1 libxkbcommon0:i386 libxrandr2 libxrandr2:i386 libxrender1 libxshmfence-dev libxslt1-dev libxss-dev libxt-dev libxtst-dev libxtst6 libxtst6:i386 linux-libc-dev-armhf-cross locales openbox p7zip patch perl pkg-config rpm rpm-common rpm2cpio rrdtool ruby subversion texinfo uuid-dev wdiff x11-utils xcompmgr xvfb xz-utils zip zlib1g lib32z1 lighttpd libncurses5:i386 libstdc++6:i386 zlib1g:i386 libsquashfuse0 squashfs-tools libfuse3-3 libfuse2 gvfs-fuse fuse3 exfat-fuse
```

---------------------------------------------------------------
Then install the sysroots for all platforms (to avoid issues), run (from chromium/src)

`build/linux/sysroot_scripts/install-sysroot.py --all`

Lastly, if you want ChromiumOS fonts for ThoriumOS development, you can run (as root)

`build/linux/install-chromeos-fonts.py`
