@ECHO OFF

REM Bldmake-generated batch file - ABLD.BAT
REM ** DO NOT EDIT **

perl -S ABLD.PL "\Series60SDK_dev\Series60_tools\EcmtServer\server\plugins\SEIagent\group\\" %1 %2 %3 %4 %5 %6 %7 %8 %9
if errorlevel==1 goto CheckPerl
goto End

:CheckPerl
perl -v >NUL
if errorlevel==1 echo Is Perl, version 5.003_07 or later, installed?
goto End

:End
