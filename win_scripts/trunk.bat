## Windows native build trunk script.
# Copyright (c) 2022 Alex313031. 

echo "Rebasing/Syncing and running hooks..." &&

cd C:\src\chromium\src\v8 &&

git checkout -f origin/main &&

cd C:\src\chromium\src &&

git checkout -f origin/main &&

git rebase-update &&

git fetch --tags &&

gclient sync --with_branch_heads --with_tags -f -R -D &&

gclient runhooks &&

echo "Downloading PGO Profile for Win64..." &&

python tools\update_pgo_profiles.py --target=win64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles &&

exit 0
