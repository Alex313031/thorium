#!/bin/bash
MACHINE="$(gcc -dumpmachine)"
echo Building...
mkdir -p "build_${MACHINE}"
cd "build_${MACHINE}"
rm -f *.pak
cmake -GNinja ..
ninja

echo Testing...
for i in $(ls ../tests | grep pak | cut -f1 -d '.')
do
./pak -u "../tests/${i}.pak" "./${i}_unpacked"
./pak -p "./${i}_unpacked/pak_index.ini" "${i}_repacked.pak"
cmp "../tests/${i}.pak" "${i}_repacked.pak" && rm -rf "./${i}_unpacked"
done
cd ..
