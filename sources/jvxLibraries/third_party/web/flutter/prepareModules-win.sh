#!/bin/bash  

if [ ! -d "flutter-3.7.4" ]; then
	curl -kLSs  https://storage.googleapis.com/flutter_infra_release/releases/stable/windows/flutter_windows_3.7.4-stable.zip -o flutter.zip	
	unzip flutter.zip
	mv flutter flutter-3.7.4
	rm flutter.zip
fi


