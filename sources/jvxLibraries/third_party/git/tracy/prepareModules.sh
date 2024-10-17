#!/bin/bash

if [ ! -d "tracy" ]; then
	git clone https://github.com/wolfpld/tracy.git
	cd tracy
	git checkout v0.11.1
fi
