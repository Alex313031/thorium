#!/bin/bash

cp -r -v build/. $HOME/chromium/src/build/ &&
cp -r -v chrome/. $HOME/chromium/src/chrome/ &&
cp -r -v components/. $HOME/chromium/src/components &&
cp -r -v content/. $HOME/chromium/src/content/ &&
cp -r -v media/. $HOME/chromium/src/media/ &&
cp -r -v net/. $HOME/chromium/src/net/ &&
cp -r -v ui/. $HOME/chromium/src/ui/ &&
cp -r -v v8/. $HOME/chromium/src/v8/ &&

cd $HOME/chromium/src &&

exit 0
