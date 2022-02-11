echo Syncing msys2 packages...
C:\msys64\usr\bin\pacman -Sq --noconfirm --needed --noprogressbar --ask=20 mingw-w64-x86_64-ninja mingw-w64-i686-ninja
rem C:\msys64\usr\bin\pacman -Scq --noconfirm

echo Fetching test files
if not exist tests mkdir tests
cd tests
for %%i in (test_v4_474896_1.pak test_v4_474896_2.pak test_v4_474896_r.pak test_v5_550886_1.pak test_v5_550886_2.pak test_v5_550886_r.pak) do if not exist "%%~i" appveyor.exe DownloadFile "https://github.com/myfreeer/chrome-pak-customizer/releases/download/1.0/%%~i"
cd ..

echo Building and testing 64-bit version...
set MSYSTEM=MINGW64
call C:\msys64\usr\bin\bash -lc "cd \"$APPVEYOR_BUILD_FOLDER\" && exec ./test.sh"
move /Y .\build_x86_64-w64-mingw32\pak.exe .\pak_mingw64.exe

echo Building and testing 32-bit version...
set MSYSTEM=MINGW32
call C:\msys64\usr\bin\bash -lc "cd \"$APPVEYOR_BUILD_FOLDER\" && exec ./test.sh"
move /Y .\build_i686-w64-mingw32\pak.exe .\pak_mingw32.exe

echo Building with msvc 32-bit...
mkdir msvc_32
cd msvc_32
cmake ..
cmake --build . --target ALL_BUILD --config Release
move /Y .\Release\pak.exe ..\pak_msvc32.exe
cd ..

echo Building with msvc 64-bit...
mkdir msvc_64
cd msvc_64
cmake -G "Visual Studio 14 2015 Win64" ..
cmake --build . --target ALL_BUILD --config Release
move /Y .\Release\pak.exe ..\pak_msvc64.exe
cd ..

echo Packaging...
7z a -mx9 chrome-pak.7z .\pak_mingw64.exe .\pak_mingw32.exe .\pack.bat .\unpack.bat
7z a -mx9 chrome-pak_msvc.7z .\pak_msvc32.exe .\pak_msvc64.exe .\pack.bat .\unpack.bat
echo Done.