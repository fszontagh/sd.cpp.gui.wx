@echo OFF & setlocal
REM %1 = ${external:stable-diffusion}
git -c core.protectNTFS=false apply --allow-empty --ignore-whitespace --ignore-space-change --unsafe-paths --whitespace=fix --quiet --directory="%1" "patches/lora_6198017.patch" || exit /b 0
exit /b 0