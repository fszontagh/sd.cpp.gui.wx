#!/bin/bash

if [ ! "$1" ]; then
    echo "Usage: $0 target_directory"
    exit 1
fi

CPP_FILE_LIST=`find ./src -name '*.cpp' -print`
xgettext --c++ -c -j -d stablediffusiongui -s --keyword=_ -p ./po -o stablediffusiongui.pot $CPP_FILE_LIST