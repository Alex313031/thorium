# PKGBUILD for Arch Linux
#
# Build-time selection:
#   _variant=avx|avx2|sse3|sse4   (default: avx2)
#
# Examples:
#   makepkg -si _variant=sse4
#   yay -S thorium-latest-bin --mflags "_variant=sse3"

pkgname=thorium-latest-bin
pkgdesc="Thorium Browser — latest Linux binary (x86_64, AVX/AVX2/SSE3/SSE4)"
pkgver=0
pkgrel=1
arch=('x86_64')
url="https://github.com/Alex313031/Thorium"
license=('BSD')
depends=('glibc' 'hicolor-icon-theme')
makedepends=('curl' 'jq' 'unzip')
conflicts=('thorium' 'thorium-bin' 'thorium-browser' 'thorium-browser-bin')
provides=('thorium' 'thorium-browser')
source=()
sha256sums=()
options=('!strip')

: "${_variant:=avx2}"

_get_latest_json() {
  local api="https://api.github.com/repos/Alex313031/Thorium/releases/latest"
  local auth=()
  [[ -n ${GH_TOKEN:-} ]] && auth=(-H "Authorization: Bearer ${GH_TOKEN}")
  curl -fsSL "${auth[@]}" -H 'Accept: application/vnd.github+json' "$api"
}

_variant_regex() {
  case "$1" in
    avx)  echo 'AVX\.zip$' ;;
    avx2) echo 'AVX2\.zip$' ;;
    sse3) echo 'SSE3\.zip$' ;;
    sse4) echo 'SSE4\.zip$' ;;
    *) return 1 ;;
  esac
}

prepare() {
  cd "$srcdir"

  case "$_variant" in
    avx|avx2|sse3|sse4) ;;
    *) echo "!! Invalid _variant=$_variant (choose avx|avx2|sse3|sse4)" >&2; exit 1 ;;
  esac

  echo ">> Variant: $_variant"
  local regex="$(_variant_regex "$_variant")"

  echo ">> Querying latest release…"
  local json url ver
  json="$(_get_latest_json)" || { echo "!! GitHub API failed"; exit 1; }

  ver="$(jq -r '.tag_name // empty' <<<"$json")"
  [[ -z $ver || $ver == null ]] && { echo "!! Could not detect tag_name"; exit 1; }

  url="$(jq -r --arg re "$regex" '.assets[] | select(.name | test($re; "i")) | .browser_download_url' <<<"$json" | head -n1)"
  [[ -z $url || $url == null ]] && { echo "!! No asset matched $_variant"; exit 1; }

  printf '%s\n' "${ver#v}" > .pkgver
  printf '%s\n' "$_variant" > .variant

  local zname="thorium-${_variant}-${ver#v}.zip"
  echo ">> Downloading: $url"
  curl -4fL --retry 5 --retry-delay 2 --retry-all-errors -o "$zname" "$url"

  echo ">> Extracting…"
  rm -rf "$srcdir/extract"
  mkdir -p "$srcdir/extract"
  unzip -q "$zname" -d "$srcdir/extract"
}

pkgver() {
  cd "$srcdir"
  [[ -s .pkgver ]] && cat .pkgver || echo "0"
}

package() {
  cd "$srcdir"
  local eff_variant="$(< .variant)"

  install -d "$pkgdir/opt/thorium-$eff_variant"
  cp -a "$srcdir/extract/." "$pkgdir/opt/thorium-$eff_variant/"

  # Find the native launcher
  local native_bin
  native_bin="$(find "$pkgdir/opt/thorium-$eff_variant" -maxdepth 2 -type f \( -name 'thorium' -o -name 'thorium-browser' \) -perm /111 | head -n1 || true)"

  install -d "$pkgdir/usr/bin"
  if [[ -n $native_bin ]]; then
    chmod +x "$native_bin"
    cat > "$pkgdir/usr/bin/thorium" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail
for cand in "/opt/THDIR/thorium" "/opt/THDIR/thorium-browser"; do
  exe="${cand/THDIR/THVAR}"
  if [[ -x "$exe" ]]; then
    exec "$exe" "$@"
  fi
done
echo "Thorium launcher not found in /opt/THDIR" >&2
exit 1
EOF
    sed -i "s|THDIR|thorium-$eff_variant|g; s|THVAR|thorium-$eff_variant|g" "$pkgdir/usr/bin/thorium"
    chmod 755 "$pkgdir/usr/bin/thorium"

    ln -sfr "$pkgdir/usr/bin/thorium" "$pkgdir/usr/bin/thorium-$eff_variant"
  else
    echo "!! No native Thorium binary found for $_variant" >&2
  fi

  # Desktop entry
  install -d "$pkgdir/usr/share/applications"
  cat > "$pkgdir/usr/share/applications/thorium.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=Thorium Browser (${eff_variant^^})
Comment=Fast Chromium fork
Exec=thorium %U
Terminal=false
Categories=Network;WebBrowser;
StartupWMClass=thorium
MimeType=text/html;x-scheme-handler/http;x-scheme-handler/https;
EOF

  # Icon (best-effort)
  install -d "$pkgdir/usr/share/icons/hicolor/256x256/apps"
  local icon
  icon="$(find "$pkgdir/opt/thorium-$eff_variant" -type f -iname '*256*.png' | head -n1 || true)"
  [[ -n $icon ]] && install -m644 "$icon" "$pkgdir/usr/share/icons/hicolor/256x256/apps/thorium.png"

  # Licenses (best-effort)
  if compgen -G "$pkgdir/opt/thorium-$eff_variant/*LICENSE*" >/dev/null; then
    install -d "$pkgdir/usr/share/licenses/$pkgname"
    cp -a "$pkgdir/opt/thorium-$eff_variant/"*LICENSE* "$pkgdir/usr/share/licenses/$pkgname/" || true
  fi
}
