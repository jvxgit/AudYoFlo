#!/bin/bash 

echo "Running on OS $OS"

iniDir=`pwd`

if [ ! -d "AudYoFlo" ]; then
	git clone $1 $2 https://github.com/jvxgit/AudYoFlo.git
fi

cd AudYoFlo/sources/sub-projects

if [ ! -d "jvx-nr" ]; then
	git clone $1 $2  git@git.binauricsaudio.com:jvxrt/audio/jvx-nr.git
fi

if [ ! -d "jvx-aec" ]; then
	git clone $1 $2 git@git.binauricsaudio.com:jvxrt/audio/jvx-aec.git
fi

if [ ! -d "jvx-diffarray" ]; then
	git clone $1 $2  git@git.binauricsaudio.com:jvxrt/audio/jvx-diffarray.git
fi

if [ ! -d "ayf-headrest" ]; then
	git clone $1 $2  git@git.binauricsaudio.com:jvxrt/audio/ayf-headrest.git
fi

if [ ! -d "ayfpae" ]; then
	git clone $1 $2  git@git.binauricsaudio.com:jvxrt/audio/ayfpae.git
fi

