#!/bin/bash  

# QT_VERSION="5.15.2" -> $1
# ARCH="gcc_64" -> $2

# aqt list-qt linux desktop --arch 5.15.2
if [ ! -d "qt/$1/$2" ]; then
	echo aqt install-qt linux desktop $1 $2 -O qt
	aqt install-qt linux desktop $1 $2 -O qt
fi
