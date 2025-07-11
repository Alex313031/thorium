#!/bin/bash

merge_extensions() {
  (
    shopt -s nullglob
    dest=/app/chromium/extensions/$1
    mkdir -p $dest
    for ext in /app/chromium/${1%/*}/$1/*; do
      ln -s $ext $dest
    done
  )
}

unlink_profiles() {
  local stamp="$XDG_CONFIG_HOME/chromium-profiles-unlinked-stamp"
  if [[ ! -d "$XDG_CONFIG_HOME/chromium" ]]; then
    touch "$stamp"
  elif [[ ! -f "$stamp" ]]; then
    unlink_profiles.py && touch "$stamp"
  fi
}

if [[ ! -f /app/chromium/extensions/no-mount-stamp ]]; then
  # Merge all legacy extension points if the symlinks had a tmpfs mounted over
  # them.
  merge_extensions native-messaging-hosts
  merge_extensions policies/managed
  merge_extensions policies/recommended
fi

# Unlink any profiles from the sync keys to avoid any expected deletions.
unlink_profiles

export LIBGL_DRIVERS_PATH=/usr/lib/$(uname -m)-linux-gnu/GL/lib/dri
exec cobalt "$@"
