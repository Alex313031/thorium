#!/usr/bin/env bash

set -euo pipefail

exec /opt/thorium-browser/thorium_shell \
	--enable-experimental-web-platform-features \
	--debug \
	--enable-clear-hevc-for-testing \
	"$@"
