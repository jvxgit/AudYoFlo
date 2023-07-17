#!/bin/bash  

if [ ! -d "libmysofa" ]; then
	git clone https://github.com/hoene/libmysofa.git

	cd libmysofa
	git checkout 0b6c96541cbabcaf4ace1a0f48d3aef548be448d

	# Really hard to patch this - patch tools are really annoying. Something with linefeeds
	patch -p1  < ../patch-src.patch.glnx
	patch -p1  < ../patch-cml.patch.glnx
	cd ..
fi
