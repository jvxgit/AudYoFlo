#!/bin/bash  

if [ ! -d "vst3sdk" ]; then
	git clone --branch v3.8.0_build_66 --recursive https://github.com/steinbergmedia/vst3sdk.git
	cd vst3sdk
	# git apply ../0001-Modified-lib-to-not-exit-in-case-a-bmp-was-not-valid.patch
	cd ..
fi
