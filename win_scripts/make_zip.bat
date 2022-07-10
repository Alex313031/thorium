@echo on

if "%1" == "" (
  echo usage: make_zip builddir
  echo.
  echo   builddir   build output directory ^(e.g., out\Release^)
  goto :eof
)

setlocal

set makezip=chrome\tools\build\make_zip.py
set outdir=%1
set cfg=chrome\tools\build\win\FILES.cfg
set archive=%outdir%\thorium-win64.zip

python3 %makezip% %outdir% %cfg% %archive%
