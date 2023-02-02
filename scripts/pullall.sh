#!/bin/bash          
 
## You may call this script as follows:
## ./pullall.sh 'GIT_SSH_COMMAND="ssh -i /o/ssh-keys/id_rsa"' <branch-name-audio> <branch-name>
exitOnError=false
errorOccured=false

run_pull()
{	
	cd $1
	echo "Folder ==> `pwd`"
	if [ -d .git ]; then
		if [ "$3" != "" ];then
			echo "git checkout $2"
			git checkout $3
			## if [[ $? -ne 0 ]]; then
			##	echo "--> Checking out $2 failed, using master instead"
			##	git checkout master
			## fi
		fi
		echo "$2 git pull --rebase"
		eval "$2 git pull --rebase"
		if ! [ $? -eq 0 ]; then
			RED='\033[0;31m'
			NC='\033[0m' # No Color
			echo -e "${RED}Git returned non-zero error code. Please correct the error and run this script again.${NC}"
			errorOccured=true
			if $exitOnError; then
				echo -e "${RED}Exiting.${NC}"
				exit 1
			fi
		fi
	fi
	cd ..
}

PROJECT_BRANCH=""
PROJECT_BRANCH_AUDIO=""

if [[ $# -gt 1 ]]; then
	PROJECT_BRANCH_AUDIO=$2
fi
if [[ $# -gt 2 ]]; then
	PROJECT_BRANCH=$3
fi

iniDir=`pwd`

PREFIX=
if [ $# -ge 1 ]; then
	if [ -n "$1" ]; then
		Q='"'
		PREFIX="GIT_SSH_COMMAND=$Q ssh -i $1 $Q"
		echo $PREFIX
	fi
fi

## =========================================================================

run_pull AudYoFlo "$PREFIX" $PROJECT_BRANCH
cd AudYoFlo/sources/sub-projects	

## ================================================

for d in ./*; do
  if [ -d "$d" ]; then
	if [ -f $d/.jvxprj ]; then
		#echo "Valid folder $d"
		run_pull $d "$PREFIX" $PROJECT_BRANCH
	fi
	if [ -f $d/.jvxprj.dep ]; then
		#echo "Valid folder $d"
		run_pull $d "$PREFIX" $PROJECT_BRANCH
	fi
	if [ -f $d/.jvxprj.base ]; then
		#echo "Valid folder $d"
		run_pull $d "$PREFIX" $PROJECT_BRANCH
	fi
	if [ -f $d/.jvxprj.audio ]; then
		#echo "Valid folder $d"
		run_pull $d "$PREFIX" $PROJECT_BRANCH
	fi
	if [ -f $d/.jvxprj.audio.dep ]; then
		#echo "Valid folder $d"
		run_pull $d "$PREFIX" $PROJECT_BRANCH
	fi
  fi
done
	
if $errorOccured; then
	echo -e "${RED}Finished. One or more errors occured.${NC}"
fi