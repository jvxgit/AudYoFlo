#!/bin/bash  

if [ ! -d "$1" ]; then
	wget https://www.fftw.org/fftw-3.3.10.tar.gz
	tar -xvzf fftw-3.3.10.tar.gz
	rm fftw-3.3.10.tar.gz
fi