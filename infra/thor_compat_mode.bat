@echo off
echo:
echo Copyright (c) 2022 Alex313031
echo:
echo Automatically sets the compatibility mode for Thorium on Windows 10/11,
echo as a mitigation to the "Error code: RESULT_CODE_MISSING_DATA" bug.
echo _________________________________________________________________________
echo:

reg.exe Add "HKLM\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" /v "%LOCALAPPDATA%\Thorium\Application\chrome.exe" /d "~ WIN8RTM"
reg.exe Add "HKCU\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" /v "%LOCALAPPDATA%\Thorium\Application\chrome.exe" /d "~ WIN8RTM"