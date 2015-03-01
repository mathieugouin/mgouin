@echo off
set OldPath=%Path%
set Path=C:\PIC_C\PICC;C:\PIC_C\PICC\DEVICES;%Path%

if "%1"=="" goto syntax_error

set SOURCE=%1

    CCSC +FM +T +A +M +L +J +Z +D +DC +Y0 +P %SOURCE%
rem CCSC +FM -T -A -M -L -J -Z -D -DC +Y9 +P %SOURCE%

goto end

:syntax_error
echo Syntax is:
echo %0 CFILE

:end

set Path=%OldPath%

@echo on
