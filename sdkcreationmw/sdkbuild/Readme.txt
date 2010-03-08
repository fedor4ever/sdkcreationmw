	How to Build the SDK


1  Download the sdkcreationmw package to your local drive


2  Download the platform sources 

3  Set up the envinoment variables as below


/*********************************************************************************************
set ARMROOT=\
set EPOCROOT=\
set CW_SYMBIAN_VERSION=3.1
set CW_HOME=C:\apps\cw
set MSDEV_HOME=C:\program files\microsoft visual studio
set ARMDLL=%CW_HOME%\Bin


rem set JAVA_1_4_HOME=C:\Program Files\Java\j2SDK1.4.2.05
set JAVA_1_4_HOME=C:\j2sdk1.4.2_05

set BUILD_DRIVE=x
subst %BUILD_DRIVE%: C:\SDK\SF_New_Struct_build\sdk_w36

set JAVA_HOME=%JAVA_1_4_HOME%


rem Add MSDEV to path
PATH=%PATH%;%MSDEV_HOME%\vc98\bin;%MSDEV_HOME%\common\ide98\bin;%MSDEV_HOME%\common\tools

rem Add Java-include dirs
set include=%include%;%JAVA_HOME%\include;%JAVA_HOME%\include\win32


%BUILD_DRIVE%:
cls


**********************************************************************************************/


4 Goto sdkbuild directory and execute the following commonds

	sdkbuild.pl -a -l