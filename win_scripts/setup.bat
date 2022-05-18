## Windows native build setup script.
# Copyright (c) 2022 Alex313031. 

echo "Creating build output directory..." &&

mkdir C:\src\chromium\src\out\thorium &&

echo "Copying Thorium source files over the Chromium tree..." &&

copy ..\ash\* C:\src\chromium\src\ash &&
copy ..\base\* C:\src\chromium\src\base &&
copy ..\build\* C:\src\chromium\src\build &&
copy ..\chrome\* C:\src\chromium\src\chrome &&
copy ..\components\* C:\src\chromium\src\components &&
copy ..\extensions\* C:\src\chromium\src\extensions &&
copy ..\content\* C:\src\chromium\src\content &&
copy ..\media\* C:\src\chromium\src\media &&
copy ..\net\* C:\src\chromium\src\net &&
copy ..\sandbox\* C:\src\chromium\src\sandbox &&
copy ..\third_party\* C:\src\chromium\src\third_party &&
copy ..\tools\* C:\src\chromium\src\tools &&
copy ..\ui\* C:\src\chromium\src\ui &&
copy ..\v8\* C:\src\chromium\src\v8 &&
copy ..\content_shell\* C:\src\chromium\src\out\thorium &&
copy ..\pak_src\bin\pak-win\* C:\src\chromium\src\out\thorium &&

set NINJA_SUMMARIZE_BUILD=1 &&

echo "Dropping you to C:\src\chromium\src" &&

cd C:\chromium\src

exit 0
