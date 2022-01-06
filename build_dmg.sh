#!/bin/bash

xattr -csr out/thorium/Chromium.app

codesign --force --deep --sign - out/thorium/Chromium.app

chrome/installer/mac/pkg-dmg \
  --sourcefile --source out/thorium/Chromium.app \
  --target "out/thorium/Thorium_MacOS.dmg" \
  --volname Thorium --symlink /Applications:/Applications \
  --format UDBZ --verbosity 2

