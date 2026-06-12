#!/bin/bash  

if [ ! -d "boost" ]; then

	if [ ! -f "boost-1.70.0.zip" ]; then
		echo "1) Downloading boost binary package - this may really take some time!!"
		# curl -kLSs https://boostorg.jfrog.io/artifactory/main/release/1.87.0/binaries/boost_1_87_0-unsupported-bin-msvc-all-32-64.7z -o boost-1.87.0.7z
		wget https://archives.boost.io/release/1.70.0/source/boost_1_70_0.zip
	fi 

	echo "2) Extracting boost binary package"
	unzip -q boost_1_70_0.zip
	
	pwd 
	ls
	
	echo "3) Renaming boost folder - multiple attempts"
	max=10
	source="boost_1_70_0"
	target="boost"

	for ((i=1; i<=max; i++)); do
		if [ -d "$target" ]; then
			echo "Target '$target' already exists; done."
			break
		fi

		echo "Trying to $source -> $target, mv -T "$source" "$target"  run <$i>"

		if mv -T "$source" "$target" 2>/dev/null; then
			echo "Rename succeeded."
			break
		else
			# optional: Fehlermeldung anzeigen
			echo "mv failed (run <$i>), retrying..." >&2
			sleep 5
		fi
	done
	
	echo "4) Removing downloaded packages"
	rm boost_1_70_0.zip
	
fi