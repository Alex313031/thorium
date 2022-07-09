# Copyright (c) 2022 Alex313031 and JPratama7
# Maintainer: JPratama7 <josepratama080@gmail.com>

pkgname=thorium-browser-unstable-bin
pkgver=104.0.5083.0
pkgrel=1
pkgdesc="Chromium fork for Linux named after radioactive element No. 90."
arch=('x86_64')
url="https://thorium.rocks"
license=('GPL3')
depends=('alsa-lib' 'ca-certificates' 'dbus' 'gtk3' 'libcups' 'nss' 'ttf-liberation' 'xdg-utils')
optdepends=('pipewire: WebRTC desktop sharing under Wayland'
            'kdialog: support for native dialogs in Plasma'
            'org.freedesktop.secrets: password storage backend on GNOME / Xfce'
            'kwallet: support for storing passwords in KWallet on Plasma')
provides=("${pkgname%-bin}")
conflicts=("${pkgname%-bin}" 'chromium')
options=('!strip' '!emptydirs')
source=("https://github.com/Alex313031/Thorium/releases/download/M${pkgver}/thorium-browser_${pkgver}-1_amd64.deb")
sha512sums=('cc0392baa4c8d866b1487ec4e740a5b810cd599848b438d7029d8268d3290336cafb68754705eba5c6362dff8dcd77759dbe476e7ace283db7027bf6108924dc')

package() {
  bsdtar -xf data.tar.xz -C "$pkgdir/"

  chmod 4755 "$pkgdir/opt/chromium.org/thorium-unstable/chrome-sandbox"

  for icon_size in 16 24 32 48 64 128 256; do
    icons_dir=usr/share/icons/hicolor/${icon_size}x${icon_size}/apps
    install -Dm644 "$pkgdir/opt/chromium.org/thorium-unstable/product_logo_${icon_size}.png" \
      "$pkgdir/${icons_dir}/${pkgname%-bin}.png"
  done
  install -Dm644 "$pkgdir/opt/chromium.org/thorium-unstable/thorium-devtools.png" -t \
    "$pkgdir/usr/share/icons/hicolor/256x256/apps/"

  rm -r \
    "$pkgdir"/etc/ \
    "$pkgdir"/opt/chromium.org/thorium-unstable/cron/ \
    "$pkgdir"/opt/chromium.org/thorium-unstable/product_logo_*.{png,xpm} \
    "$pkgdir"/usr/share/menu/
}
