@echo ON
setlocal
REM %1 source dll
REM %2 taget dir
set "source_dll=%1"
set "target_dir=%2"

set "source_dll=%source_dll:/=\%"
set "target_dir=%target_dir:/=\%"


copy /Y "%source_dll%" "%target_dir%/"

endlocal