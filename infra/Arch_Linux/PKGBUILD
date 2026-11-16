# Maintainer: @RubenKelevra <rubenkelevra@gmail.com>
# Contributor: xiota <aur@mentalfossa.com>
# Contributor: JPratama7 <josepratama080@gmail.com>
# Contributor: Dominik Adrian Grzywak <starterx4 at gmail dot com>

_pkgname='thorium-browser'
pkgname="${_pkgname}-beta-bin"
pkgbase="${pkgname}"
pkgver=156.0.8078.28
pkgrel=2
pkgdesc='Beta build of the Chromium fork focused on performance and security'
url='https://github.com/gz83/thorium'
license=(
	'BSD-3-Clause'
	'LicenseRef-Google-Widevine-CDM'
)
arch=(
	'aarch64'
	'i686'
	'x86_64'
)
makedepends=('libarchive')
depends=(
	'alsa-lib'
	'at-spi2-core'
	'cairo'
	'dbus'
	'expat'
	'glib2'
	'gtk3'
	'hicolor-icon-theme'
	'libcups'
	'libgcc'
	'libnotify'
	'libx11'
	'libxcb'
	'libxcomposite'
	'libxdamage'
	'libxext'
	'libxfixes'
	'libxkbcommon'
	'libxrandr'
	'mesa'
	'nspr'
	'nss'
	'pango'
	'systemd-libs'
	'ttf-liberation'
	'xdg-utils'
)
optdepends=(
	'gtk4: GTK4 UI support'
	'kdialog: native dialogs on Plasma'
	'libsecret: password storage client integration'
	'org.freedesktop.secrets: Secret Service backend for password storage'
	'pipewire: WebRTC desktop sharing under Wayland'
	'qt5-base: Qt5 UI support'
	'qt6-base: Qt6 UI support'
	'upower: Battery Status API support'
)
provides=(
	"${_pkgname}=${pkgver}"
	"${_pkgname}-beta=${pkgver}"
)
conflicts=(
	"${_pkgname}"
	"${_pkgname}-bin"
	"${_pkgname}-sse3-bin"
	"${_pkgname}-sse4-bin"
	"${_pkgname}-avx-bin"
	"${_pkgname}-avx2-bin"
	'thorium-browser-beta-sse4-bin'
	'thorium-browser-beta-avx-bin'
	'thorium-browser-beta-avx2-bin'
	'thorium-browser-beta-avx512-bin'
)
options=(
	'!emptydirs'
	'!strip'
	'!debug'
)

_dl_url="${url}/releases/download/M${pkgver}"
_dl_filename_aarch64="${_pkgname}_${pkgver}_arm64.deb"
_dl_filename_i686="${_pkgname}_${pkgver}_i386.deb"
_dl_filename_x86_64="${_pkgname}_${pkgver}_SSE3.deb"
_license_filename="${_pkgname}-${pkgver}-LICENSE.md"
_data_archive='data.tar.xz'
noextract=(
	"${_dl_filename_aarch64}"
	"${_dl_filename_i686}"
	"${_dl_filename_x86_64}"
)

source=(
	"${_pkgname}.sh"
	'thorium-cpu-check.sh'
	'thorium-shell.sh'
	"${_license_filename}::${url}/raw/M${pkgver}/LICENSE.md"
)
source_aarch64=("${_dl_url}/${_dl_filename_aarch64}")
source_i686=("${_dl_url}/${_dl_filename_i686}")
source_x86_64=("${_dl_url}/${_dl_filename_x86_64}")
b2sums=(
	'038ce007f34bc9c519bcc1918ca472cf6596a1dade80b9b150021762a51cf28bc331bb82299110023593f569b9cf285c9ed60d695cfd2abf82d0bf9073ceeee5'
	'49417ddc79a842674a08f0e6ec6975a14cec3b2ae7536b0ddeaa68189045f42851f9f1fab8110889b4488db3c2bd58c9b071b8b95be3db8b99eb92abc2c89465'
	'867bc169f526a530a79d1eef5cc463d1c386d380bb574228a46492c9c85fc5dcdaa8768e08282dbe5ed660458613b67da8e3f822a3c9d93866e2252f3332b672'
	'fe17f16ad6a8911dc9c49de3e45fa450a2a3ef59ec2f5400daed74b4401cfe2426e5b52e8e2e519bbc9f2b110112e616578eaf57705605a75cc5e8fa9d820060'
)
sha256sums_aarch64=('1fa1a1d73b6a3da2b81b6afce5273733910600d36c0fc04c7600306733dfe863')
sha256sums_i686=('b7c0fbd59b64a50353d1fc037ca98f9f66066c2a69e61bdeba939df2fca8004d')
sha256sums_x86_64=('f712a8e5475c4baeec1f84ac2a8efd3f03f4fe2c5bed75e4454761ad54cdb77e')

prepare() {
	CARCH="${CARCH}" bash "${srcdir}/thorium-cpu-check.sh" build
}

package() {
	local dl_filename
	case ${CARCH} in
		aarch64)
			dl_filename=${_dl_filename_aarch64}
			;;
		i686)
			dl_filename=${_dl_filename_i686}
			;;
		x86_64)
			dl_filename=${_dl_filename_x86_64}
			;;
		*)
			printf 'Unsupported architecture: %s\n' "${CARCH}" >&2
			return 1
			;;
	esac

	printf '  -> Extracting the archive...\n'
	rm -f -- "${srcdir}/${_data_archive}"
	bsdtar -C "${srcdir}" -xf "${srcdir}/${dl_filename}" "${_data_archive}"
	bsdtar -C "${pkgdir}/" -xJf "${srcdir}/${_data_archive}"
	rm -f -- "${srcdir}/${_data_archive}"

	printf '  -> Moving files in place...\n'
	mv -- "${pkgdir}/opt/chromium.org/thorium" "${pkgdir}/opt/${_pkgname}"
	unlink -- "${pkgdir}/usr/bin/thorium-browser"
	unlink -- "${pkgdir}/usr/bin/pak"

	# Upstream labels its beta packages as stable in the version dialog.
	# Correct the channel without changing the CPU variant annotation.
	local upstream_launcher="${pkgdir}/opt/${_pkgname}/${_pkgname}"
	sed -E \
		-e 's@^([[:space:]]*export CHROME_VERSION_EXTRA=")stable@\1beta@' \
		-i -- "${upstream_launcher}"
	if ! grep -Eq '^[[:space:]]*export CHROME_VERSION_EXTRA="beta' "${upstream_launcher}" ||
		grep -Eq '^[[:space:]]*export CHROME_VERSION_EXTRA="stable' "${upstream_launcher}"; then
		printf 'Unexpected upstream release channel marker: %s\n' "${upstream_launcher}" >&2
		return 1
	fi

	# thorium-browser
	install -Dm755 -- "${srcdir}/${_pkgname}.sh" "${pkgdir}/usr/bin/${_pkgname}"
	install -Dm755 -- "${srcdir}/thorium-cpu-check.sh" \
		"${pkgdir}/usr/lib/${_pkgname}/check-cpu-support"
	chmod 4755 -- "${pkgdir}/opt/${_pkgname}/chrome-sandbox"

	local widevine_dir="${pkgdir}/opt/${_pkgname}/WidevineCdm"
	if [[ ${CARCH} == i686 || ${CARCH} == aarch64 ]]; then
		# No supported Widevine CDM is bundled for Linux i686 or arm64.
		# Keep both architectures Widevine-free, even if upstream adds one.
		local license_index
		for license_index in "${!license[@]}"; do
			if [[ ${license[license_index]} == 'LicenseRef-Google-Widevine-CDM' ]]; then
				unset 'license[license_index]'
			fi
		done
		rm -rf -- "${widevine_dir}"

		local clearkey_platform='linux_x86'
		if [[ ${CARCH} == aarch64 ]]; then
			clearkey_platform='linux_arm64'
		fi

		local clearkey_native="${pkgdir}/opt/${_pkgname}/ClearKeyCdm/_platform_specific/${clearkey_platform}/libclearkeycdm.so"
		if [[ ! -f ${clearkey_native} ]]; then
			printf 'Missing %s ClearKey CDM: %s\n' "${CARCH}" "${clearkey_native}" >&2
			return 1
		fi
		install -Dm644 -- "${clearkey_native}" "${pkgdir}/opt/${_pkgname}/lib/libclearkeycdm.so"
		rm -rf -- "${pkgdir}/opt/${_pkgname}/ClearKeyCdm/_platform_specific/linux_x64"
	else
		if [[ ! -f ${widevine_dir}/_platform_specific/linux_x64/libwidevinecdm.so || ! -f ${widevine_dir}/LICENSE ]]; then
			printf 'Missing bundled x86_64 Widevine CDM or license\n' >&2
			return 1
		fi
		install -Dm644 -- "${widevine_dir}/LICENSE" \
			"${pkgdir}/usr/share/licenses/${pkgname}/Widevine-LICENSE"
	fi
	# Fix upstream paths after moving Thorium out of /opt/chromium.org.
	if [[ -f ${pkgdir}/opt/${_pkgname}/default-app-block ]]; then
		sed -E \
			-e "s@/opt/chromium.org/thorium/@/opt/${_pkgname}/@g" \
			-i -- "${pkgdir}/opt/${_pkgname}/default-app-block"
	fi
	# Bring upstream AppStream metadata up to the current schema and path.
	local metainfo_source="${pkgdir}/usr/share/appdata/thorium-browser.appdata.xml"
	local metainfo_file="${pkgdir}/usr/share/metainfo/org.chromium.Thorium.metainfo.xml"
	if [[ ! -f ${metainfo_source} ]]; then
		printf 'Missing Thorium AppStream metadata: %s\n' "${metainfo_source}" >&2
		return 1
	fi
	install -Dm644 -- "${metainfo_source}" "${metainfo_file}"
	rm -f -- "${metainfo_source}"
	rmdir --ignore-fail-on-non-empty -- "${pkgdir}/usr/share/appdata"
	sed -E \
		-e 's@<id>thorium-browser[.]desktop</id>@<id>org.chromium.Thorium</id>@' \
		-e '/<translation\/>/d' \
		-e 's@<developer_name>([^<]+)</developer_name>@<developer id="io.github.alex313031"><name>\1</name></developer>@' \
		-i -- "${metainfo_file}"
	if ! grep -q '<launchable type="desktop-id">' "${metainfo_file}"; then
		sed -i '/<url type="bugtracker">/i\  <launchable type="desktop-id">thorium-browser.desktop</launchable>' \
			"${metainfo_file}"
	fi
	if ! grep -q '<content_rating type="oars-1.1"' "${metainfo_file}"; then
		sed -i '/<url type="bugtracker">/i\  <content_rating type="oars-1.1"/>' \
			"${metainfo_file}"
	fi

	# Keep the alternate desktop ID available without creating a duplicate launcher.
	local primary_desktop="${pkgdir}/usr/share/applications/thorium-browser.desktop"
	local alias_desktop="${pkgdir}/usr/share/applications/org.chromium.Thorium.desktop"
	if [[ ! -f ${primary_desktop} || ! -f ${alias_desktop} ]]; then
		printf 'Missing Thorium browser desktop entry\n' >&2
		return 1
	fi
	sed -i \
		-e '/^NoDisplay=/d' \
		-e '/^\[Desktop Entry\]$/a NoDisplay=true' \
		-- "${alias_desktop}"
	if ! grep -Fxq 'NoDisplay=true' "${alias_desktop}" ||
		grep -Eq '^(NoDisplay|Hidden)=true$' "${primary_desktop}"; then
		printf 'Expected exactly one visible Thorium browser launcher\n' >&2
		return 1
	fi

	# thorium-shell
	install -Dm755 -- "${srcdir}/thorium-shell.sh" "${pkgdir}/usr/bin/thorium-shell"
	sed -E \
		-e 's@^Icon=.*@Icon=thorium-shell@' \
		-i -- "${pkgdir}/usr/share/applications/thorium-shell.desktop"

	# thorium-browser.xml
	sed -E \
		-e "s@/opt/chromium.org/thorium/@/opt/${_pkgname}/@" \
		-i -- "${pkgdir}/usr/share/gnome-control-center/default-apps/thorium-browser.xml"

	# Icons
	install -Dm644 -- "${pkgdir}/opt/${_pkgname}/product_logo_256.png" \
		"${pkgdir}/usr/share/icons/hicolor/256x256/apps/${_pkgname}.png"
	install -Dm644 -- "${pkgdir}/opt/${_pkgname}/thorium_shell.png" \
		"${pkgdir}/usr/share/icons/hicolor/256x256/apps/thorium-shell.png"

	# Clean up upstream Debian packaging files.
	rm -rf -- \
		"${pkgdir}/opt/chromium.org" \
		"${pkgdir}/etc/cron.daily/" \
		"${pkgdir}/usr/share/doc/" \
		"${pkgdir}/opt/${_pkgname}/cron/" \
		"${pkgdir}/opt/${_pkgname}"/product_logo_*.{png,xpm} \
		"${pkgdir}/usr/share/menu/"

	install -Dm644 -- "${srcdir}/${_license_filename}" \
		"${pkgdir}/usr/share/licenses/${pkgname}/LICENSE.md"
}
