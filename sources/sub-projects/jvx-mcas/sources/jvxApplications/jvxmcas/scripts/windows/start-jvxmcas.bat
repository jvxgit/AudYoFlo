@ECHO OFF
REM ================================================================
REM Start host...
REM One optional parameter: A config file may be specified directly, e.g.,
REM "jvxmcas configfile.jvx"
REM ================================================================
set PATH=C:\Windows\System32\Npcap;.\lib\;%PATH%
IF "%~1"=="" GOTO startnormal
GOTO startspecific
:startnormal
bin\jvxmcas.exe  --jvxdir jvxComponents --config jvxmcas.jvx
goto endscript
:startspecific
bin\jvxmcas.exe
:endscript
REM ENDSCRIPT
