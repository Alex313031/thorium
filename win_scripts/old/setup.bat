## Windows native build setup script.
# Copyright (c) 2022 Alex313031. 

echo "Creating build output directory..." &&

mkdir C:\src\chromium\src\out\thorium &&

echo "Copying Thorium source files over the Chromium tree..." &&

copy ..\src\BUILD.gn C:\src\chromium\src\ &&
copy ..\src\ash\* C:\src\chromium\src\ash &&
copy ..\src\build\* C:\src\chromium\src\build &&
copy ..\src\chrome\* C:\src\chromium\src\chrome &&
copy ..\src\components\* C:\src\chromium\src\components &&
copy ..\src\content\* C:\src\chromium\src\content &&
copy ..\src\extensions\* C:\src\chromium\src\extensions &&
copy ..\src\media\* C:\src\chromium\src\media &&
copy ..\src\net\* C:\src\chromium\src\net &&
copy ..\src\sandbox\* C:\src\chromium\src\sandbox &&
copy ..\src\third_party\* C:\src\chromium\src\third_party &&
copy ..\src\tools\* C:\src\chromium\src\tools &&
copy ..\src\ui\* C:\src\chromium\src\ui &&
copy ..\src\v8\* C:\src\chromium\src\v8 &&
copy ..\src\thorium_shell\* C:\src\chromium\src\out\thorium &&
copy ..\src\pak_src\bin\pak-win\* C:\src\chromium\src\out\thorium &&

set NINJA_SUMMARIZE_BUILD=1 &&

echo "Dropping you to C:\src\chromium\src" &&

cd C:\src\chromium\src
