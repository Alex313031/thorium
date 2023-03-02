#!/bin/bash
set -euxo pipefail

# Get pkgname
PKG=$(awk -F '=' '/^pkgname/{ print $2 }' PKGBUILD)

# Get latest version
VER=$(curl -sSf https://dl.thorium.rocks/debian/dists/stable/main/binary-amd64/Packages.gz |
	grep -A1 "Package: ${PKG}" |
	awk '/Version/{print $2}' |
	cut -d '-' -f1)

# Insert latest version into PKGBUILD and update hashes
sed -i \
	-e "s/^pkgver=.*/pkgver=${VER}/" \
	PKGBUILD

# Check whether this changed anything
if (git diff --exit-code PKGBUILD); then
	echo "Package ${PKG} has most recent version ${VER}"
	exit 0
fi

sed -i \
	-e 's/pkgrel=.*/pkgrel=1/' \
	PKGBUILD

updpkgsums

# Update .SRCINFO
makepkg --printsrcinfo >.SRCINFO

# Commit changes
git add PKGBUILD .SRCINFO
git commit -m "${PKG} v${VER}"
