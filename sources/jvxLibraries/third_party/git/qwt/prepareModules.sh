#!/bin/bash  

if [ ! -d "qwt" ]; then
	git clone -b qwt-6.2 https://github.com/mbdevpl/qwt.git
	patch -p0 < patch-compile-windows-16012023.patch
fi
