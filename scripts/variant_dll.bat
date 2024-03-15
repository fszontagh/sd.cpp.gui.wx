@ECHO OFF
REM %1 source
REM %2 variant
setlocal

set "source_dir=%1"
set "source_dir=%source_dir:/=\%"

copy "%source_dir%\stable-diffusion.dll" "%source_dir%\stable-diffusion_%2.dll" /Y

endlocal