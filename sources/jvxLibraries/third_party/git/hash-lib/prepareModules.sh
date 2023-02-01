#!/bin/bash  

if [ ! -d "hash-library" ]; then
	git clone https://github.com/stbrumme/hash-library
	cd hash-library
	git checkout d389d18112bcf7e4786ec5e8723f3658a7f433d7
	cd ..
fi
