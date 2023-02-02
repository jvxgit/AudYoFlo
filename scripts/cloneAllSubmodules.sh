#!/bin/bash 

#
# When using an alternative ssh key, run the script as
# ./cloneAllSubmodules /path/to/.ssh/id_rsa_key <optional: branch-expression>
# It will run then
# GIT_SSH_COMMAND="ssh -i /path/to/.ssh/id_rsa_key" git clone <optional: branch-expression> https://github.com/jvxgit/AudYoFlo.git
#

echo "Running on OS $OS"

iniDir=`pwd`

PREFIX=
if [ $# -ge 1 ]; then
	Q='"'
	PREFIX="GIT_SSH_COMMAND=$Q ssh -i $1 $Q"
	echo $PREFIX
fi

if [ ! -d "AudYoFlo" ]; then
	eval $PREFIX git clone $1 $2 https://github.com/jvxgit/AudYoFlo.git
fi


