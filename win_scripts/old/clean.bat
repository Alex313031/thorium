# Copyright (c) 2022 Alex313031.

# Run from within out\Thorium or wherever you put your build, or cd there first
# i.e. cd C:\src\chromium\src\out\thorium

echo "Cleaning up build artifacts..." &&

cd C:\src\chromium\src\out\thorium &&

del *deps* &&
del *TOC* &&
rmdir /s /q pyproto &&
rmdir /s /q obj &&
rmdir /s /q newlib_pnacl_nonsfi &&
rmdir /s /q newlib_pnacl &&
rmdir /s /q nacl_bootstrap_x64 &&
rmdir /s /q irt_x64 &&
rmdir /s /q glibc_x64 &&
rmdir /s /q gen &&
rmdir /s /q etc &&
rmdir /s /q clang_newlib_x64 &&
rmdir /s /q thinlto-cache &&
rmdir /s /q fontconfig_caches &&

echo "Done!"
