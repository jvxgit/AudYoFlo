#!/bin/bash          
 
## You may call this script as follows:
## Command usage example:
# ./pushallnb.sh 'GIT_SSH_COMMAND="ssh -i /o/ssh-keys/id_rsa"' <new-branch>
run_push()
{	
	cd $1
	echo "Folder ==> `pwd`"
	if [ -d .git ]; then
		eval $2 git checkout -b $3 
		eval $2 git push --set-upstream origin $3
	fi
	cd ..
}

iniDir=`pwd`
cd AudYoFlo/sources/sub-projects

for d in ./*; do
  if [ -d "$d" ]; then
	if [ -f $d/.jvxprj ]; then
		#echo "Valid folder $d"
		run_push $d "$1" "$2"
	fi
	if [ -f $d/.jvxprj.dep ]; then
		#echo "Valid folder $d"
		run_push $d "$1" "$2"
	fi
	if [ -f $d/.jvxprj.base ]; then
		#echo "Valid folder $d"
		run_push $d "$1" "$2"
	fi
	if [ -f $d/.jvxprj.audio ]; then
		#echo "Valid folder $d"
		run_push $d "$1" "$2"
	fi
	if [ -f $d/.jvxprj.audio.dep ]; then
		#echo "Valid folder $d"
		run_push $d "$1" "$2"
	fi
  fi
done


