#!/bin/bash  

if [ ! -d "qcustomplot" ]; then
	curl -kLSs https://www.qcustomplot.com/release/2.1.0fixed/QCustomPlot.tar.gz -o qcustomplot.tar.gz
	tar -xvzf qcustomplot.tar.gz
	cd qcustomplot
	patch -p1 < ../qcustomplot-2-0-1-patch-16012023.patch
	cd ..
fi
