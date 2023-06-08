#!/bin/bash  

if [ ! -d "libmysofa" ]; then
	git clone https://github.com/hoene/libmysofa.git

	cd libmysofa
	git checkout  bed445ba48ad9faf0e758b54af3f6075949d27fe
	cd ..
	
	# Really hard to patch this - patch tools are really annoying. Something with linefeeds
	patch -R -p0  --binary < patch-src.patch.win
	patch -R -p0  --binary < patch-cml.patch.win
	
	cp -r libs/zlib libmysofa/windows/third-party
fi
