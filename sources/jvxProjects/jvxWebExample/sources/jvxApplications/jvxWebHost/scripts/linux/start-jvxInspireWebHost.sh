#!/bin/sh
export LD_LIBRARY_PATH=./bin/:$LD_LIBRARY_PATH
bin/jvxInspireWebHost  --config jvxInspireWebHost.jvx --textlog --verbose_dll --textloglev 10
