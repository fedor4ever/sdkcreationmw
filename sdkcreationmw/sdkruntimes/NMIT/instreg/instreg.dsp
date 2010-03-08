# Microsoft Developer Studio Project File - Name="instreg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=instreg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "instreg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "instreg.mak" CFG="instreg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "instreg - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "instreg - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "instreg - Win32 WinRel" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "instreg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build/release"
# PROP Intermediate_Dir "build/release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../NmitShared/src/native/slib/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
TargetPath=.\build\release\instreg.exe
TargetName=instreg
SOURCE="$(InputPath)"
PostBuild_Desc=Copying release files...
PostBuild_Cmds=if exist ..\..\..\..\epoc32\release\wins\urel echo ..\..\..\..\epoc32\release\wins\urel\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\wins\urel copy $(TargetPath) ..\..\..\..\epoc32\release\wins\urel\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\winscw\urel echo ..\..\..\..\epoc32\release\winscw\urel\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\winscw\urel copy $(TargetPath) ..\..\..\..\epoc32\release\winscw\urel\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\winscw\udeb echo ..\..\..\..\epoc32\release\winscw\udeb\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\winscw\udeb copy $(TargetPath) ..\..\..\..\epoc32\release\winscw\udeb\$(TargetName).exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "instreg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build/debug"
# PROP Intermediate_Dir "build/debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../NmitShared/src/native/slib/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\build\debug\instreg.exe
TargetName=instreg
SOURCE="$(InputPath)"
PostBuild_Desc=Copying debug files...
PostBuild_Cmds=if exist ..\..\..\..\epoc32\release\wins\udeb echo ..\..\..\..\epoc32\release\wins\udeb\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\wins\udeb copy $(TargetPath) ..\..\..\..\epoc32\release\wins\udeb\$(TargetName).exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "instreg - Win32 WinRel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "instreg___Win32_Release_Windows"
# PROP BASE Intermediate_Dir "instreg___Win32_Release_Windows"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build/winrel"
# PROP Intermediate_Dir "build/winrel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../../NmitShared/src/native/slib/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../NmitShared/src/native/slib/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib slib.lib expat.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /machine:I386 /out:"build/winrel/instregw.exe" /libpath:"../../../NmitShared/src/native/slib" /libpath:"../../../NmitShared/src/native/expat/build/release"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=.\build\winrel\instregw.exe
TargetName=instregw
SOURCE="$(InputPath)"
PostBuild_Desc=Copying release files...
PostBuild_Cmds=if exist ..\..\..\..\epoc32\release\wins\urel echo ..\..\..\..\epoc32\release\wins\urel\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\wins\urel copy $(TargetPath) ..\..\..\..\epoc32\release\wins\urel\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\winscw\urel echo ..\..\..\..\epoc32\release\winscw\urel\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\winscw\urel copy $(TargetPath) ..\..\..\..\epoc32\release\winscw\urel\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\winscw\udeb echo ..\..\..\..\epoc32\release\winscw\udeb\$(TargetName).exe	if exist ..\..\..\..\epoc32\release\winscw\udeb copy $(TargetPath) ..\..\..\..\epoc32\release\winscw\udeb\$(TargetName).exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "instreg - Win32 Release"
# Name "instreg - Win32 Debug"
# Name "instreg - Win32 WinRel"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\instreg.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\instreg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\instreg.rc
# End Source File
# End Group
# End Target
# End Project
