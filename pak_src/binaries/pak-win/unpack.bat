@echo off
setlocal EnableExtensions EnableDelayedExpansion
if exist "%SystemRoot%\SysWOW64\cmd.exe" set x64=1
set core_x64=pak_mingw64.exe
set core_x86=pak_mingw32.exe
set "WORK_DIR=%~dp0"
set "file=%~1"
set unpack_dir=%~dpn1_unpacked
if not exist "%WORK_DIR%" md "%WORK_DIR%"
pushd "%WORK_DIR%"
if "%x64%"=="1" ( set core=%core_x64% ) else ( set core=%core_x86% )
if not exist %core% echo core program (%core%) NOT FOUND! &pause &exit
if not exist "%unpack_dir%" md "%unpack_dir%"
if exist "%file%" %core% -u "%file%" "%unpack_dir%"
if exist "%unpack_dir%\pak_index.ini" echo Unpacked "%file%" to "%unpack_dir%"
timeout /t 5||pause