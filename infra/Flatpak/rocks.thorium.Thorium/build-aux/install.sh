#!/bin/bash -ex

mkdir -p /app/chromium

pushd out/Release
for path in chrome chrome_crashpad_handler icudtl.dat *.so libvulkan.so.1 *.pak *.bin *.png locales MEIPreload vk_swiftshader_icd.json; do
    # All the 'libVk*' names are just for debugging, stuff like "libVkICD_mock_icd" and "libVkLayer_khronos_validation".
    [[ "$path" == libVk* ]] && continue
    cp -rv $path /app/chromium
done
popd

# Place the proprietary libffmpeg in the extension path, then overwrite it with the free one.
install -Dm 755 out/ReleaseFree/libffmpeg.so /app/chromium/libffmpeg.so
install -Dm 755 out/Release/libffmpeg.so /app/chromium/nonfree-codecs/lib/libffmpeg.so

for size in 24 48 64 128 256; do
    install -Dvm 644 chrome/app/theme/chromium/linux/product_logo_$size.png /app/share/icons/hicolor/${size}x${size}/apps/rocks.thorium.Thorium.png;
done
install -Dvm 644 cobalt.ini -t /app/etc
install -Dvm 644 rocks.thorium.Thorium.desktop -t /app/share/applications
install -Dvm 644 rocks.thorium.Thorium.metainfo.xml -t /app/share/metainfo
install -Dvm 755 chromium.sh /app/bin/chromium
install -Dvm 755 unlink_profiles.py -t /app/bin
