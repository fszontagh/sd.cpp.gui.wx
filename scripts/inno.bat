@echo OFF
REM %1 = ${buildDir}
REM %2 = variation, avx, avx2, cuda
REM %3 = ${distributionDir}
REM %4 = ${cwd}


set "version_file=%1/int/ver.hpp"
set "version_file=%version_file:/=\%"

set "installer_dir=%3"
set "installers_dir=%installer_dir:/=\%"



for /f tokens^=2^ delims^=^" %%a in ('type %version_file% ^| findstr /c:"SD_GUI_VERSION"') do (
    set "SD_GUI_VERSION=%%~a"
)

set ISCC="C:\Program Files (x86)\Inno Setup 6\iscc.exe"
set "SD_GUI_BUILD=%3"
set "SD_GUI_BUILD=%SD_GUI_BUILD:/=\%"
set "SD_GUI_BUILD=..\%SD_GUI_BUILD%"
set "SD_GUI_BUILD=%~dp0%SD_GUI_BUILD%"
set "SD_GUI_BUILD=%SD_GUI_BUILD%\%2"

if exist "%SD_GUI_BUILD%\COMCTL32.dll" (
    del "%SD_GUI_BUILD%\COMCTL32.dll"
)


set "RELEASE_DIR=%1"
set "RELEASE_DIR=%RELEASE_DIR:/=\%"
set "RELEASE_DIR=..\%RELEASE_DIR%"
set "RELEASE_DIR=%~dp0%RELEASE_DIR%"
set "RELEASE_DIR=%RELEASE_DIR%"

echo "RELEASE_DIR=%RELEASE_DIR%"
echo "SD_GUI_BUILD=%SD_GUI_BUILD%"

for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set datetime=%%I
set custom_date=%datetime:~0,4%-%datetime:~4,2%-%datetime:~6,2%

set SD_GUI_VARIATION=%2
set SD_GUI_VERSION=%SD_GUI_VERSION%

echo SD GUI version: %SD_GUI_VERSION%
echo SD GUI variation: %SD_GUI_VARIATION%

REM force copy cuda if it is a cuda
if "%SD_GUI_VARIATION%" == "all" (
    copy /Y %CUDA_PATH%\bin\cublas64_12.dll "%SD_GUI_BUILD%\"
    copy /Y %CUDA_PATH%\bin\cublasLt64_12.dll "%SD_GUI_BUILD%\"
    copy /Y %CUDA_PATH%\bin\cudart64_12.dll "%SD_GUI_BUILD%\"
    copy /Y "%RELEASE_DIR%\*.dll" "%SD_GUI_BUILD%\"
    copy /Y "%RELEASE_DIR%\stable-diffusion_cuda.dll" "%SD_GUI_BUILD%\"
    copy /Y "%RELEASE_DIR%\stable-diffusion_rocm.dll" "%SD_GUI_BUILD%\"
    copy /Y "%RELEASE_DIR%\stable-diffusion_avx.dll" "%SD_GUI_BUILD%\"
    copy /Y "%RELEASE_DIR%\stable-diffusion_avx2.dll" "%SD_GUI_BUILD%\"
    copy /Y "%RELEASE_DIR%\stable-diffusion_avx512.dll" "%SD_GUI_BUILD%\"
)

 
%ISCC% /O"%installers_dir%/%custom_date%" scripts/sd.gui.wx.iss