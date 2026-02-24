#!/bin/bash  

# $1: ${targetfolder}
# $2: ${archtoken} 
# $3: ${CMAKE_BUILD_TYPE}


if [ ! -d "$1" ]; then
	git clone https://github.com/ValveSoftware/steam-audio.git
	sleep 4
	
	mv steam-audio $1
	pushd $1
	
	git checkout 9920bda53ec0ffc2e37fc12c9e9f52af792ed924
	
	echo apply ../0001-Modification-Build-MSVC-with-MD.patch
	git apply ../0001-Modification-Build-MSVC-with-MD.patch
	cd core/build
	
	# Release build
	echo python get_dependencies.py
	python get_dependencies.py
		
	echo python build.py --minimal -o install
	python build.py --minimal -o install
	
	if [[ "$3" == "Debug" ]]
	then
		
		# Debug build
		echo python get_dependencies.py --debug
		python get_dependencies.py --debug
		
		echo python build.py --minimal -c Debug -o install
		python build.py --minimal -c Debug -o install
		
	fi
		
	popd
fi
