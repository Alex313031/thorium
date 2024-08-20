#!/bin/bash

# Copyright (c) 2024 Alex313031

export SHELL_WRAPPER="`readlink -f "$0"`"

HERE="`dirname "$SHELL_WRAPPER"`"

PROFILE="$HERE/.config/thorium-shell"
CACHE="$HERE/.config/cache"
export PROFILE
export CACHE

# Launch thorium_shell
# Note: exec -a below is a bashism.
exec -a "$0" "$HERE/thorium_shell" "--user-data-dir=$PROFILE" "--disk-cache-dir=$CACHE" "--enable-experimental-web-platform-features" "--enable-clear-hevc-for-testing" "$@"
