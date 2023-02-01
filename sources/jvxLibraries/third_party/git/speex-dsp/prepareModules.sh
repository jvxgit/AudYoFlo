#!/bin/bash  

if [ ! -d "speexdsp" ]; then
	git clone https://github.com/xiph/speexdsp.git
	cd speexdsp
	git checkout SpeexDSP-1.2rc3
	cd ..
fi
