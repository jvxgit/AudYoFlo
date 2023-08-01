#!/bin/bash  

if [ ! -d "qcustomplot" ]; then
	curl -kLSs https://www.qcustomplot.com/release/2.1.1/QCustomPlot.tar.gz -o qcustomplot.tar.gz
	tar -xvzf qcustomplot.tar.gz
	cd qcustomplot
	patch -p1 < ../qcustomplot-2-1-0-patch-01082023.patch
	cd ..
fi
