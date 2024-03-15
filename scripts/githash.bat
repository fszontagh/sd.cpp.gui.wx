@echo off

for /f "delims=" %%i in ('git rev-parse --short HEAD') do set GIT_HASH=%%i
if exist .env (
    findstr /C:"GIT_HASH=" .env > nul
    if errorlevel 1 (
        echo GIT_HASH=%GIT_HASH%>> .env
    ) else (
        for /f "tokens=1,* delims==" %%a in ('type .env ^| findstr /V /B "GIT_HASH="') do (
            echo %%a=%%b>> .env.tmp
        )
        echo GIT_HASH=%GIT_HASH%>> .env.tmp
        move /y .env.tmp .env > nul
    )
) else (
    echo GIT_HASH=%GIT_HASH%> .env
)