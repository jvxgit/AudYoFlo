#!/bin/bash  

if [ ! -d "vst3sdk" ]; then
	echo git clone --recursive --branch v3.7.2_build_28  https://github.com/steinbergmedia/vst3sdk.git
	git clone --recursive --branch v3.7.2_build_28  https://github.com/steinbergmedia/vst3sdk.git

	git apply -p0 ../../patches/vst/adapt_vst_repo-CMakeLists.patch
	git apply -p0 ../../patches/vst/adapt_vst_repo-PluginInterfaces.patch
fi



