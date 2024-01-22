#!/bin/bash

# Copyright (c) 2024 Alex313031.

echo "Installs necessary prerequisites for building and running Chromium/ChromiumOS on Arch Linux."

sudo pacman -Syyuu --needed autoconf autoconf-archive automake base-devel beep bluez-libs cabextract cmake curl dkms dosfstools exfatprogs exo ffmpeg gcc git go gperf gtk2 gtk3 hwdata i2c-tools java-runtime-common java-runtime-headless kdialog libcbor libdrm libnet libpulse libsecret libudev0-shim libva libva-utils libva-vdpau-driver libwebp libxss lm_sensors lsb-release make man-db mesa-utils minizip mtools nano nasm ncurses nodejs nss ntfs-3g numlockx openjpeg2 opus org.freedesktop.secrets p7zip pciutils pipewire polkit python python-docutils python-oauth2client python-oauthlib python-pkgconfig python-pkginfo python-protobuf python-setuptools python-virtualenv qt5-base re2 read-edid tar libtar tk tree ttf-font ttf-liberation unrar unzip vulkan-extra-layers vulkan-headers vulkan-tools wget xdg-utils xsensors xz yasm zenity
