## Windows native build setup script.
# Copyright (c) 2022 Alex313031. 

mkdir C:\chromium\src\out\thorium

copy ..\build\* C:\chromium\src\build
copy ..\chrome\* C:\chromium\src\chrome
copy ..\components\* C:\chromium\src\components
copy ..\content\* C:\chromium\src\content
copy ..\media\* C:\chromium\src\media
copy ..\net\* C:\chromium\src\net
copy ..\sandbox\* C:\chromium\src\sandbox
copy ..\third_party\* C:\chromium\src\third_party
copy ..\tools\* C:\chromium\src\tools
copy ..\ui\* C:\chromium\src\ui
copy ..\v8\* C:\chromium\src\v8
copy ..\content_shell\* C:\chromium\src\out\thorium
copy ..\pak_src\bin\pak-win\* C:\chromium\src\out\thorium

cd C:\chromium\src

exit 0
