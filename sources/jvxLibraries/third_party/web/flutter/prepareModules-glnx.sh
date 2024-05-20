#!/bin/bash  

if [ ! -d "flutter-3.7.4" ]; then

	curl -kLSs  https://storage.googleapis.com/flutter_infra_release/releases/stable/linux/flutter_linux_3.7.4-stable.tar.xz -o flutter.tar.xz	
	xz -d flutter.tar.xz
	tar -xf flutter.tar 
	sleep 1
	mv flutter flutter-3.7.4
	rm flutter.tar
fi


