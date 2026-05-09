#!/bin/bash -ex

# Needed to build GN itself.
. /usr/lib/sdk/llvm14/enable.sh

# GN will use these variables to configure its own build, but they introduce
# compat issues w/ Clang and aren't used by Chromium itself anyway, so just
# unset them here.
unset CFLAGS CXXFLAGS LDFLAGS

if [[ -d third_party/llvm-build/Release+Asserts/bin ]]; then
  # The build scripts check that the stamp file is present, so write it out
  # here.
  PYTHONPATH=tools/clang/scripts/ \
    python3 -c 'import update; print(update.PACKAGE_VERSION)' \
    > third_party/llvm-build/Release+Asserts/cr_build_revision
else
  python3 tools/clang/scripts/build.py --disable-asserts \
      --skip-checkout --use-system-cmake --use-system-libxml \
      --gcc-toolchain=/usr \
      --host-cc=/usr/lib/sdk/llvm14/bin/clang \
      --host-cxx=/usr/lib/sdk/llvm14/bin/clang++ \
      --without-android --without-fuchsia \
      --with-ml-inliner-model=
fi

# (TODO: enable use_qt in the future?)
# DO NOT REUSE THE BELOW API KEY; it is for Flathub only.
# http://lists.debian.org/debian-legal/2013/11/msg00006.html
tools/gn/bootstrap/bootstrap.py -v --no-clean --gn-gen-args='
    use_sysroot=false
    use_lld=true
    enable_nacl=false
    blink_symbol_level=0
    use_gnome_keyring=false
    use_pulseaudio=true
    clang_use_chrome_plugins=false
    is_official_build=true
    google_api_key="AIzaSyAL6fqCZVFhA7K_qBvz9GO5Z-V1JBcPO0A"
    treat_warnings_as_errors=false
    proprietary_codecs=true
    ffmpeg_branding="Chrome"
    is_component_ffmpeg=true
    use_vaapi=true
    enable_widevine=true
    rtc_use_pipewire=true
    rtc_link_pipewire=true
    enable_hangout_services_extension=true
    disable_fieldtrial_testing_config=true
    use_system_libwayland=false
    use_system_libffi=true
    use_qt=false
    enable_remoting=false
'
mkdir -p out/ReleaseFree
cp out/Release{,Free}/args.gn
echo -e 'proprietary_codecs = false\nffmpeg_branding = "Chromium"' >> out/ReleaseFree/args.gn
out/Release/gn gen out/ReleaseFree
