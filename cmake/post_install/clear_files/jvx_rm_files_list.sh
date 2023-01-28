#!/bin/bash

# ================================================================
# Run this script as follows:
# ./jvx_rm_header_all.sh ../build/win-make-rtproc64f/log.2017-01-24.txt
# It should remove all javox headers from the files specified in 
# ../build/win-make-rtproc64f/log.2017-01-24.txt
# The file to specify which files to remove can be obtained by
# doing a "" and then special selection of the entries 
# Get the list of files as follows:
# -> find runtime -type f > rm_files_rt.txt
# ================================================================

# Read in all previously modified files
filenames=()

if [ -f "$1" ]
then
	dos2unix $1
	while IFS='' read -r line || [[ -n "$line" ]]; do
		filenames+=("$line")
	done < "$1"

	# Remove the header comments 
	for i in ${filenames[@]}; do

		myfile=$2/$i
	
		if [ -d "$myfile" ]; then
		
			echo "-------> Removing directory $myfile"
			rm -rf $myfile
		elif [ -f "$myfile" ]; then	
	
			echo "-------> Removing file $myfile"
			rm $myfile
		fi
	done
fi

	

	

