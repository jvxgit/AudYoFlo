#!/bin/bash  

if [ ! -d "asiosdk_2.3.3_2019-06-14" ]; then
	
	curl -kLSs https://www.steinberg.net/asiosdk?_ga=2.34212833.1787253143.1617615529-938406583.1617615529asiosdk_2.3.3_2019-06-14.zip -o asiosdk_2.3.3_2019-06-14.zip
	unzip asiosdk_2.3.3_2019-06-14.zip	
fi
