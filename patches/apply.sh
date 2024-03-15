#!/bin/bash

if [ ! "$1" ]; then 
    exit 1;
fi;

git -c core.protectNTFS=false apply --ignore-whitespace --ignore-space-change --unsafe-paths --whitespace=fix --directory="$1" "patches/lora_6198017.patch" 2>/dev/null|| exit 0
exit 0