@ECHO OFF
REM ================================================================
REM Start host...
REM One optional parameter: A config file may be specified directly, e.g.,
REM "jvxDarwinWebHost configfile.jvx"
REM ================================================================
set PATH=.\bin\;%PATH%
IF "%~1"=="" GOTO startnormal
GOTO startspecific
:startnormal
bin\jvxInspireWebHost.exe  --config jvxInspireWebHost.jvx --textlog --verbose_dll --textloglev 10
goto endscript
:startspecific
bin\jvxDarwinWebHost.exe
:endscript
REM ENDSCRIPT
