#!/bin/bash  

if [ ! -d "flutter-3.7.4" ]; then

	if [ ! -d "flutter" ]; then
		curl -kLSs  https://storage.googleapis.com/flutter_infra_release/releases/stable/windows/flutter_windows_3.7.4-stable.zip -o flutter.zip	
		unzip flutter.zip
	fi 
	
	sleep 5
	
	max=10
	for i in {1..$max}
	do
		if [ ! -d "flutter-3.7.4" ]; then
			echo "Trying to mv flutter to flutter-3.7.4, rum <$i>"
			mv flutter flutter-3.7.4
			if [ ! -d "flutter-3.7.4" ]; then
				sleep 5
			fi 
		fi 
	done
		
	rm flutter.zip
fi


