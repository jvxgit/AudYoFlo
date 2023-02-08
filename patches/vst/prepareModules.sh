#!/bin/bash  

if [ ! -d "vst3sdk" ]; then
	git clone --recursive --branch v3.7.7_build_19 https://github.com/steinbergmedia/vst3sdk.git
	
	patch -R -p0 --binary < ../../patches/vst/patch.vstsdk-v3.7.7-cml.patch
	patch -R -p0 --binary < ../../patches/vst/patch.vstsdk-v3.7.7-cma.patch
fi



