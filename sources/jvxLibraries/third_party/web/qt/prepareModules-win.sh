#!/bin/bash  

QT_VERSION="5.15.2"

# $1 should be win64_msvc2019_64 or win32_msvc2019
# aqt list-qt windows desktop --arch 5.15.2
if [ ! -d "qt" ]; then
	curl -kLSs  https://github.com/miurahr/aqtinstall/releases/download/v3.1.6/aqt.exe -o aqt.exe
	 
	echo ./aqt.exe install-qt windows desktop $QT_VERSION $1 -O qt
	./aqt.exe install-qt windows desktop $QT_VERSION $1 -O qt

fi
