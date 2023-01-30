#!/bin/bash          
 
## You may call this script as follows:
## ./pushall.sh 'GIT_SSH_COMMAND="ssh -i /o/ssh-keys/id_rsa"' 
run_status()
{	
	cd $1
	echo "Folder ==> `pwd`"
	#eval $2 git status 
	if [ -d .git ]; then
		eval $2 git status | grep "Your branch"
		eval $2 git status | grep modified
		eval $2 git status | grep new
	fi
	cd ..
}

iniDir=`pwd`
cd AudYoFlo/sources/sub-projects

for d in ./*; do
  if [ -d "$d" ]; then
	if [ -f $d/.jvxprj ]; then
		#echo "Valid folder $d"
		run_status $d "$1"
	fi
	if [ -f $d/.jvxprj.dep ]; then
		#echo "Valid folder $d"
		run_status $d "$1"
	fi
	if [ -f $d/.jvxprj.base ]; then
		#echo "Valid folder $d"
		run_status $d "$1"
	fi
	if [ -f $d/.jvxprj.audio ]; then
		#echo "Valid folder $d"
		run_status $d "$1"
	fi
	if [ -f $d/.jvxprj.audio.dep ]; then
		#echo "Valid folder $d"
		run_status $d "$1"
	fi
  fi
done

