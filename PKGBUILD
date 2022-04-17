# Maintainer: JPratama7 <josepratama080@gmail.com>

pkgbase=thorium-browser-unstable
pkgname=${pkgbase}-bin
pkgver=101.0.4949.0
pkgrel=1
pkgdesc="The web browser from Chromiumorg and The Thorium Authors. Thorium is a browser that combines a minimal design with sophisticated technology to make the web faster, safer, and easier."
arch=('x86_64')
url=""
license=('GPL3')
depends=('gtk3' 'nss' 'alsa-lib' 'xdg-utils' 'libxss' 'libcups' 'libgcrypt'
         'ttf-liberation' 'libxtst' 'dbus' 'libpulse' 'pciutils' 'libva'
         'desktop-file-utils' 'hicolor-icon-theme')
optdepends=(
	'pipewire: WebRTC desktop sharing under Wayland'
	'kdialog: for file dialogs in KDE'
	'gnome-keyring: for storing passwords in GNOME keyring'
	'kwallet: for storing passwords in KWallet'
    'org.freedesktop.secrets: password storage backend on GNOME / Xfce'
    'kwallet: support for storing passwords in KWallet on Plasma'
)
provides=('chromium')
options=('!strip' '!emptydirs')
source_x86_64=("https://github.com/Alex313031/Thorium/releases/download/M${pkgver}/thorium-browser_${pkgver}-${pkgrel}_amd64.deb")
sha512sums_x86_64=('216ec81322ef3a8963e0844e24a78830c44811768bc6446f652df91317e3df40d68146c74f94fb064b97292677327f7e2ea8835ac985f5fa931b1aeeb2580d90')

package(){
	# Extract package data
	tar xf data.tar.xz -C "${pkgdir}"

    chmod 4755 "$pkgdir/opt/chromium.org/thorium-unstable/chrome-sandbox"

    rm -r "$pkgdir"/etc

	# install icons
    for i in 16x16 24x24 32x32 48x48 64x64 128x128 256x256; do
        install -Dm644 "${pkgdir}"/opt/chromium.org/thorium-unstable/product_logo_${i/x*}.png \
                       "${pkgdir}"/usr/share/icons/hicolor/${i}/apps/thorium-browser-unstable.png
    done
}