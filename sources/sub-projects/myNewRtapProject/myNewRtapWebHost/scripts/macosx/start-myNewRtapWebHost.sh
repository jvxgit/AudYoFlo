#!/bin/sh
export LD_LIBRARY_PATH=./bin/:$LD_LIBRARY_PATH
bin/myNewRtapWebHost  --config myNewRtapApplication.jvx --web-num-threads 8 --textlog --verbose_dll --textloglev 10
