#!/usr/bin/env bash

set -euo pipefail

/usr/lib/thorium-browser/check-cpu-support runtime

name=thorium
flags_file="${XDG_CONFIG_HOME:-$HOME/.config}/${name}-flags.conf"

lines=()
if [[ -f $flags_file ]]; then
    mapfile -t lines < "$flags_file"
fi

flags=()
for line in "${lines[@]}"; do
    if [[ $line =~ [^[:space:]] ]] && [[ ! $line =~ ^[[:space:]]*#.* ]]; then
        flags+=("$line")
    fi
done

exec /opt/thorium-browser/thorium-browser "${flags[@]}" "$@"
