#!/bin/bash  

if [ ! -d "freeglut" ]; then
	git clone https://github.com/freeglut/freeglut.git
	cd freeglut
		git checkout b789505b4eed2ef5a5a7069e1539c6f6472a7dd3
	cd ..
fi

