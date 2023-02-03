#!/bin/bash  

if [ ! -d "libmysofa" ]; then
	git clone https://github.com/hoene/libmysofa.git

	# Really hard to patch this - patch tools are really annoying. Something with linefeeds
	patch -p0  < patch-src.patch.win
	patch -p0  < patch-cml.patch.win
	
	cp -r libs/zlib libmysofa/windows/third-party
fi
