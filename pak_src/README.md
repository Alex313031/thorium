# chrome-pak-customizer
a simple batch tool to customize pak files in chrome or chromium-based browser

## Badges
[![Build status](https://ci.appveyor.com/api/projects/status/4ji3guqh4yyjfiaa?svg=true)](https://ci.appveyor.com/project/myfreeer/chrome-pak-customizer)
[![Download](https://img.shields.io/github/downloads/myfreeer/chrome-pak-customizer/total.svg)](https://github.com/myfreeer/chrome-pak-customizer/releases)
[![Latest Release](https://img.shields.io/github/release/myfreeer/chrome-pak-customizer.svg)](https://github.com/myfreeer/chrome-pak-customizer/releases/latest)

## License
* Windows builds are licensed `LGPL 2.1+` **by default**, can be disabled by appending `-DLGPL=OFF` to cmake.
* Non-windows builds and builds with `-DLGPL=OFF` are licensed MIT.

## Usage (Windows Only)
1. Download `chrome-pak.7z` from [releases](https://github.com/myfreeer/chrome-pak-customizer/releases) and extract it to any folder.
2. Drag the pak file and drop it to `unpack.bat`.
3. The pak file would be extracted to subfolder of the folder of the pak file,
with the same name of the pak file with an `_unpacked` suffix.
4. (Optional) modify extracted files.
5. Drag `pak_index.ini` to `pack.bat`.
6. Repacked pak file would be in the same folder of `pak_index.ini` with a name like `pak_index_packed.pak`.

There is also a [GIF Guide](https://github.com/myfreeer/chrome-pak-customizer/wiki/GIF-Guide-v2) on windows.

## Usage (Command line)
```
pak.exe -u pak_file destination_path
Unpack chrome pak file at pak_file to destination_path.

pak.exe -p pak_index_file destination_pak_file
Pack chrome pak file using pak_index_file to destination_pak_file.
pak_index_file would be found in unpacked path.

Note: existing destination files would be overwritten
```

## Building
### Prerequires
* gcc
* cmake
* ninja

### Build script example
```bash
# Getting source
git clone https://github.com/myfreeer/chrome-pak-customizer --branch=develop --single-branch
cd chrome-pak-customizer

# Creating folder for build
mkdir -p build
cd build

# Running cmake
cmake -GNinja -DLGPL=OFF ..

# Building
ninja
```
### CMake options
* `LGPL`: Enable LGPL 2.1+ licensed custom startfiles on windows for smaller binary

## Credits
* <https://www.chromium.org/>
* <https://github.com/shuax/ChromePAK>
* <https://github.com/myfreeer/win-startup-headers>
