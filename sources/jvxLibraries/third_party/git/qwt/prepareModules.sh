#!/bin/bash  

if [ ! -d "qwt" ]; then
	git clone -b develop https://git.code.sf.net/p/qwt/git qwt
	git checkout 9e554a5e03bc3a1fe379bdddfa4d21ced2ed7995
	# remove below line and patch file if new qwt version works out of the box
	## patch -p0 < patch-compile-windows-16012023.patch
fi
