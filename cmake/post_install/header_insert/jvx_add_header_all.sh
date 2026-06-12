#/bin/sh

SEARCH_FOLDER="../"
INSERT_HEADER="inserted_header.txt"

echo "Right before blacklist"

BLACKLIST=(c674x-dsplib_1_03_00_01 third_party generated)

if [ "$#" -ge 3 ]; then 
	BLACKLIST=$3
fi

if [ "$#" -ge 2 ]; then 
	INSERT_HEADER=$2
fi

if [ "$#" -ge 1 ]; then
    SEARCH_FOLDER=$1
fi

echo "--> Adding Javox project header to all released header and c/cpp files"
echo "  ** Blacklist: ${BLACKLIST[*]}; insert Header: ${INSERT_HEADER}; search folder: ${SEARCH_FOLDER}"

LOG_POSTFIX=$(date "+%Y-%m-%d")
LOGFILE="log.$LOG_POSTFIX.ins.txt"

if [ -e $LOGFILE ]; then
	rm $LOGFILE
fi

files=`find "$SEARCH_FOLDER" -name "*.cpp" -or -name "*.c" -or -name "*.h"`
for f in $files	
do
	filename=$(basename "$f")
	directoryname=$(dirname "$f")
	extension="${filename##*.}"
	filename_no_ext="${filename%.*}"
	
	isinblacklist="no"
	for i in ${BLACKLIST[@]}; do
		if [[ $directoryname == *"$i"* ]]; then
			#echo "$directoryname deactivated by blacklist" 
			isinblacklist="yes"			
		fi
	done
	
	if [ $isinblacklist == "no" ]; then
		#echo "Use this: $filename which was found in $directoryname"
		echo "$directoryname/$filename" >> $LOGFILE
		
		echo "Removing header in $filename"
		cat $f | sed "/\\*J\\*.*\\*X\\*/d" > tmp.txt
		mv tmp.txt $f
		
		echo "Inserting header in $filename"
		cat ${INSERT_HEADER} | sed s/JVX_FILENAME/$filename/g > tmp.txt
		cat $f >> tmp.txt
		mv tmp.txt $f
	fi
done
