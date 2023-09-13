#!/bin/bash  

if [ ! -d "portaudio" ]; then

	git  clone https://github.com/PortAudio/portaudio.git
	cd portaudio
	git checkout  929e2e8f7af281c5eb4fa07758930d542ec43d97 
	cd ..

	patch -R -p0 --binary < patch-cml.patch
	
	# This patch to avoid symbols in ksmedia conflicting with strmiids in windows
	patch -R -p0 --binary < patch-wdmks.patch
	
	curl -kLSs https://www.steinberg.net/asiosdk?_ga=2.34212833.1787253143.1617615529-938406583.1617615529asiosdk_2.3.3_2019-06-14.zip -o asiosdk_2.3.3_2019-06-14.zip
	unzip asiosdk_2.3.3_2019-06-14.zip		
	mv asiosdk_2.3.3_2019-06-14 portaudio/src/hostapi/asio/ASIOSDK
	rm asiosdk_2.3.3_2019-06-14.zip

fi
