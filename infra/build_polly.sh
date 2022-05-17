#!/bin/bash

## Copyright (c) 2022 Alex313031/

## Clones latest LLVM being used by the Chromium Project, and builds a local toolchain with Polly to use the Polly optimizations in the main Thorium BUILD.gn

cd ~/chromium/src

python3 tools/clang/scripts/build.py --without-android --without-fuchsia --disable-asserts --gcc-toolchain=/usr --bootstrap --thinlto
