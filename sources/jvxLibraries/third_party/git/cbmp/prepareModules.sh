#!/bin/bash  

if [ ! -d "cbmt" ]; then
	git clone https://github.com/mattflow/cbmp.git
	cd cbmt
	git apply ../0001-Modified-lib-to-not-exit-in-case-a-bmp-was-not-valid.patch
	cd ..
fi
