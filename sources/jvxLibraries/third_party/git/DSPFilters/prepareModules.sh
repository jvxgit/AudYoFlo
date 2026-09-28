#!/bin/bash  

if [ ! -d "DSPFilters" ]; then

	git clone https://github.com/vinniefalco/DSPFilters.git
	cd DSPFilters
	
	git checkout acc49170e79a94fcb9c04b8a2116e9f8dffd1c7d

	git apply ../0001-Bugfix-for-latest-Windows-version.patch
	git apply ../0002-Do-not-force-MT-or-Release-build-type.patch

	cd ..
fi