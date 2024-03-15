#!/bin/bash
CPP_FILE_LIST=`find ../src -name '*.cpp' -print`
xgettext -d sd_gui_cpp -s --keyword=_ -p ./po -o sd_gui_cpp.pot $CPP_FILE_LIST