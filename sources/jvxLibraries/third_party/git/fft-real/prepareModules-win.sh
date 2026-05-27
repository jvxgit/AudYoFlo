#!/bin/bash  

if [ ! -d "fft-real" ]; then
	git clone https://github.com/cyrilcode/fft-real.git
	cd fft-real
	# git checkout 8cc6a8e5bf8bd6cfb7e9c92958a7af555301293f
	cd ..
fi

