#!/bin/bash  

if [ ! -d "DSPFilters" ]; then

	git clone https://github.com/vinniefalco/DSPFilters.git
	cd DSPFilters
	
	git checkout acc49170e79a94fcb9c04b8a2116e9f8dffd1c7d
	
	git apply ../0001-Bugfix-for-latest-Windows-version.patch
	
	cd ..
fi