#!/bin/bash  

if [ ! -d "qwt" ]; then
	git clone -b develop https://git.code.sf.net/p/qwt/git qwt
	git checkout 9e554a5e03bc3a1fe379bdddfa4d21ced2ed7995
	# with the latest version, no patch is required!
	## patch -p0 < patch-src.patch
fi
