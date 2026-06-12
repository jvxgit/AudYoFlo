#!/bin/bash  

if [ ! -d "opencv" ]; then
	git clone https://github.com/opencv/opencv.git
	cd opencv
		# git checkout 1ca526dcdb9c30600c70537e279f0c672057a1b9
		git checkout cadcb7e4e0a04867a0996a35c62960de8aa6da47 
	cd ..
fi

