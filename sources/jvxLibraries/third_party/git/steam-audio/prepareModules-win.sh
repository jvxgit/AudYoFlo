#!/bin/bash  

# $1: ${targetfolder}
# $2: ${archtoken} 
# $3: ${CMAKE_BUILD_TYPE}


if [ ! -d "$1" ]; then
	git clone https://github.com/ValveSoftware/steam-audio.git
	Sleep 4
	
	mv steam-audio $1
	pushd $1
	
	git checkout 9920bda53ec0ffc2e37fc12c9e9f52af792ed924
	
	echo apply ../0001-Modification-Build-MSVC-with-MD.patch
	git apply ../0001-Modification-Build-MSVC-with-MD.patch
	cd core/build
	
	# Release build
	echo python get_dependencies.py -p windows -t vs2022 --sharedcrt
	python get_dependencies.py -p windows -t vs2022 --sharedcrt
		
	echo python build.py -p windows -t vs2022 --sharedcrt --minimal -o install
	python build.py -p windows -t vs2022 --sharedcrt --minimal -o install
	
	if [[ "$3" == "Debug" ]]
	then
		
		# Debug build
		echo python get_dependencies.py -p windows -t vs2022 --sharedcrt --debug
		python get_dependencies.py -p windows -t vs2022 --sharedcrt --debug
		
		echo python build.py -p windows -t vs2022 --sharedcrt --minimal -c Debug -o install
		python build.py -p windows -t vs2022 --sharedcrt --minimal -c Debug -o install
		
	fi
		
	popd
fi
