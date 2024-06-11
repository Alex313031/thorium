#!/bin/bash -ex

ln_overwrite_all() {
  rm -rf "$2"
  ln -s "$1" "$2"
}

# Link our verisons of Node and OpenJDK into Chromium so the build scripts will
# use them. For OpenJDK especially, this is a workaround for:
# https://bugs.chromium.org/p/chromium/issues/detail?id=1192875
ln_overwrite_all /usr/lib/sdk/node16 third_party/node/linux/node-linux-x64
ln_overwrite_all /usr/lib/sdk/openjdk11 third_party/jdk/current

ninja -C out/ReleaseFree -j$FLATPAK_BUILDER_N_JOBS libffmpeg.so
ninja -C out/Release -j$FLATPAK_BUILDER_N_JOBS chrome chrome_crashpad_handler
