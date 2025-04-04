#!/bin/bash  

if [ ! -d "libmysofa" ]; then
	git clone https://github.com/hoene/libmysofa.git

	cd libmysofa
	
	git checkout  bed445ba48ad9faf0e758b54af3f6075949d27fe
	
	git apply ../0001-Added-cmake-option-to-activate-log-output.patch
	cd ..
fi
