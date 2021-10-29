#!/bin/bash

mkdir -v -p $HOME/chromium/src/out/thorium/ &&

cp -r -v build/. $HOME/chromium/src/build/ &&
cp -r -v chrome/. $HOME/chromium/src/chrome/ &&
cp -r -v components/. $HOME/chromium/src/components &&
cp -r -v content/. $HOME/chromium/src/content/ &&
cp -r -v media/. $HOME/chromium/src/media/ &&
cp -r -v net/. $HOME/chromium/src/net/ &&
cp -r -v ui/. $HOME/chromium/src/ui/ &&
cp -r -v v8/. $HOME/chromium/src/v8/ &&
cp -v chrome-devtools/chrome-devtools.svg $HOME/chromium/src/out/thorium/ &&
cp -v content_shell/. $HOME/chromium/src/out/thorium/ &&

cd $HOME/chromium/src &&

exit 0
