#!/bin/bash

# Copyright (c) 2022 Alex313031.

rm -r -f -v ./build &&

mkdir -v -p build &&

cd build &&

cmake -GNinja -DLGPL=OFF .. &&

ninja -v -d stats &&

cd ../build
