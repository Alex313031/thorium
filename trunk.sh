#!/bin/bash

cd /media/alex/cr/chromium/src/v8/ &&

git checkout -f origin/main &&

cd $HOME/chromium/src &&

git checkout -f origin/main &&

git rebase-update &&

git fetch --tags &&

gclient sync -D &&

gclient sync --with_branch_heads --with_tags &&

gclient runhooks &&

python tools/update_pgo_profiles.py --target=linux update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&

exit 0
