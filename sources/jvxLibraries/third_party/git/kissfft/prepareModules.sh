#!/bin/bash  

if [ ! -d "kissfft" ]; then
	git clone https://github.com/mborgerding/kissfft.git
	cd kissfft
	# git checkout 8cc6a8e5bf8bd6cfb7e9c92958a7af555301293f
	cd ..
fi

