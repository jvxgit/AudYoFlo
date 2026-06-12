#!/bin/bash  

if [ ! -d "glew" ]; then
	curl -kLSs  https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip -o glew-2.2.0.zip
	unzip glew-2.2.0.zip 
	mv glew-2.2.0 glew
	rm glew-2.2.0.zip
fi
