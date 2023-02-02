#!/bin/bash          
 
## You may call this script as follows:
## ./pushall.sh 'GIT_SSH_COMMAND="ssh -i /o/ssh-keys/id_rsa"' 
run_commid()
{	
	cd $1
	echo "Folder ==> `pwd`"
	if [ -d .git ]; then
		#eval $2 git status 
		eval git rev-parse --short HEAD
		eval git rev-parse HEAD
	fi
	cd ..
}

iniDir=`pwd`
cd AudYoFlo/sources/sub-projects

for d in ./*; do
  if [ -d "$d" ]; then
	if [ -f $d/.jvxprj ]; then
		#echo "Valid folder $d"
		run_commid $d "$1"
	fi
	if [ -f $d/.jvxprj.dep ]; then
		#echo "Valid folder $d"
		run_commid $d "$1"
	fi
	if [ -f $d/.jvxprj.base ]; then
		#echo "Valid folder $d"
		run_commid $d "$1"
	fi
	if [ -f $d/.jvxprj.audio ]; then
		#echo "Valid folder $d"
		run_commid $d "$1"
	fi
	if [ -f $d/.jvxprj.audio.dep ]; then
		#echo "Valid folder $d"
		run_commid $d "$1"
	fi
  fi
done
