@ECHO OFF
REM ================================================================
REM Start host...
REM One optional parameter: A config file may be specified directly, e.g.,
REM ================================================================
set PATH=.\bin\;%PATH%
IF "%~1"=="" GOTO startnormal
GOTO startspecific
:startnormal
bin\myNewRtapWebHost.exe  --config myNewRtapApplication.jvx --web-num-threads 8 --textlog --verbose_dll --textloglev 10
goto endscript
:startspecific
bin\jvxHoaRenderWebHost.exe
:endscript
REM ENDSCRIPT
