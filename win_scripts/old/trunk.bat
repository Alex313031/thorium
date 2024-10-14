## Windows native build trunk script.
# Copyright (c) 2022 Alex313031. 

echo "Rebasing/Syncing and running hooks..." &&

cd C:\src\chromium\src\v8 &&

git checkout -f origin/main &&

cd C:\src\chromium\src\third_party\devtools-frontend\src

git checkout -f origin/main &&

rm C:\src\chromium\src\ui\webui\resources\images\infra.png &&

rm C:\src\chromium\src\components\flags_ui\resources\hazard.svg &&

rm C:\src\chromium\src\components\neterror\resources\favicon-16x16.png &&

rm C:\src\chromium\src\components\neterror\resources\favicon-32x32.png &&

rm C:\src\chromium\src\content\shell\app\thorium_shell.ico &&

rm C:\src\chromium\src\chrome\browser\thorium_flag_entries.h &&

cd C:\src\chromium\src &&

git checkout -f origin/main &&

git rebase-update &&

git fetch --tags &&

gclient sync --with_branch_heads --with_tags -f -R -D &&

gclient runhooks &&

echo "Downloading PGO Profile for Win64..." &&

python tools\update_pgo_profiles.py --target=win64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles
