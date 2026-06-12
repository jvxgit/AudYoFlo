#!/bin/bash  

if [ ! -d "npcap-sdk" ]; then
	curl -kLSs  https://npcap.com/dist/npcap-sdk-1.13.zip -o npcap-sdk.zip
	gunzip npcap-sdk.zip
	mkdir npcap-sdk
	cd npcap-sdk
	unzip ../npcap-sdk.zip
	cd ..	
	rm npcap-sdk.zip
fi
