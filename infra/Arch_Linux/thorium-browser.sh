#!/bin/bash

XDG_CONFIG_HOME=${XDG_CONFIG_HOME:-~/.config}

# Allow users to override command-line options
if [[ -f $XDG_CONFIG_HOME/thorium-flags.conf ]]; then
   THORIUM_USER_FLAGS="$(cat $XDG_CONFIG_HOME/thorium-flags.conf)"
fi

# Launch
exec /opt/chromium.org/thorium/thorium-browser $THORIUM_USER_FLAGS "$@"
