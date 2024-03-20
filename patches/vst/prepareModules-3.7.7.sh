#!/bin/bash

if [ ! -d "vst3sdk" ]; then
	echo git clone --recursive --branch v3.7.7_build_19 https://github.com/steinbergmedia/vst3sdk.git
	git clone --recursive --branch v3.7.7_build_19 https://github.com/steinbergmedia/vst3sdk.git

	echo patch -R -p0 --binary < ../../patches/vst/patch.vstsdk-v3.7.7-cml.patch
	patch -R -p0 --binary < ../../patches/vst/patch.vstsdk-v3.7.7-cml.patch

	echo patch -R -p0 --binary < ../../patches/vst/patch.vstsdk-v3.7.7-cma.patch
	patch -R -p0 --binary < ../../patches/vst/patch.vstsdk-v3.7.7-cma.patch

	echo patch -R -p0 --binary < ../../patches/vst/patch.vstsdk-v3.7.7-modutil-cml.patch
	patch -R -p0 --binary < ../../patches/vst/patch.vstsdk-v3.7.7-modutil-cml.patch

	echo patch -p0 --binary < ../../patches/vst/patch.vstsdk-v3.7.7-modinfo.patch
	patch -p0 --binary < ../../patches/vst/patch.vstsdk-v3.7.7-modinfo.patch

	echo patch -p1 < ../../patches/vst/0001-include-cstdint.patch
	patch -p1 < ../../patches/vst/0001-include-cstdint.patch || exit 1
fi
