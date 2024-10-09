#!/bin/bash  

if [ ! -d "cbmp" ]; then
	git clone https://github.com/mattflow/cbmp.git
	cd cbmp
	git apply ../0001-Modified-lib-to-not-exit-in-case-a-bmp-was-not-valid.patch
	cd ..
fi
