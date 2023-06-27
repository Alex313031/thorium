@echo off
setlocal EnableExtensions EnableDelayedExpansion
if exist "%SystemRoot%\SysWOW64\cmd.exe" set x64=1
set core_x64=pak_mingw64.exe
set core_x86=pak_mingw32.exe
set "WORK_DIR=%~dp0"
set "file=%~1"
set "pack_file=%~dpn1_packed.pak"
if not exist "%WORK_DIR%" md "%WORK_DIR%"
pushd "%WORK_DIR%"
if "%x64%"=="1" ( set core=%core_x64% ) else ( set core=%core_x86% )
if not exist %core% echo core program (%core%) NOT FOUND! &pause &exit
if exist "%file%" %core% -p "%file%" "%pack_file%"
if exist "%pack_file%" echo packed "%pack_file%" from "%file%"
timeout /t 5||pause