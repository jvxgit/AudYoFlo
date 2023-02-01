#!/bin/bash  

if [ ! -d "eaqual" ]; then

	git clone https://github.com/jvxgit/eaqual-clone.git
	mv eaqual-clone eaqual
fi
