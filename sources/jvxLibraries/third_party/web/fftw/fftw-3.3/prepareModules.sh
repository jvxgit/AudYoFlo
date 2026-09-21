#!/bin/bash

# $1: fftw folder/version to download, e.g. fftw-3.3.11
targetfolder=${1:-fftw-3.3.11}

if [ ! -d "$targetfolder" ]; then
	wget https://www.fftw.org/$targetfolder.tar.gz || exit 1
	tar -xvzf $targetfolder.tar.gz || exit 1
	rm $targetfolder.tar.gz
fi
