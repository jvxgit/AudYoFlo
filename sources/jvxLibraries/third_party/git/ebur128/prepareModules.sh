#!/bin/bash  

if [ ! -d "libebur128" ]; then
	git clone https://github.com/jiixyj/libebur128.git

	# Really hard to patch this - patch tools are really annoying. Something with linefeeds
	patch -R -p0 --binary < patch-ebur128.patch
	patch -R -p0 --binary < patch-test.patch
fi
