#!/bin/bash  

if [ ! -d "eigen" ]; then
	echo "Cloning into eigen.."
	git clone https://gitlab.com/libeigen/eigen.git --branch 3.3.4
fi
