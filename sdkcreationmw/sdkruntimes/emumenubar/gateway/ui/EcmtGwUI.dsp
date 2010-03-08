# Microsoft Developer Studio Project File - Name="EcmtGwUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=EcmtGwUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EcmtGwUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EcmtGwUI.mak" CFG="EcmtGwUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EcmtGwUI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "EcmtGwUI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EcmtGwUI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build\Release"
# PROP Intermediate_Dir "build\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../group" /I "res" /I "../engine/include" /I "../../../../NmitShared/src/native/slib/include" /I "../../../../NmitShared/src/native/wlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comctl32.lib ws2_32.lib /nologo /base:"0x19680000" /entry:"mainCRTStartup" /subsystem:windows /machine:I386 /out:"build\Release/EcmtGw.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "EcmtGwUI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build\Debug"
# PROP Intermediate_Dir "build\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../group" /I "res" /I "../engine/include" /I "../../../../NmitShared/src/native/slib/include" /I "../../../../NmitShared/src/native/wlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comctl32.lib ws2_32.lib /nologo /base:"0x19680000" /entry:"mainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"build\Debug/EcmtGwD.exe" /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\build\Debug\EcmtGwD.exe
TargetName=EcmtGwD
SOURCE="$(InputPath)"
PostBuild_Cmds=if exist ..\..\..\..\..\epoc32\tools\ecmt echo ..\..\..\..\..\epoc32\tools\ecmt\$(TargetName).exe	if exist ..\..\..\..\..\epoc32\tools\ecmt copy $(TargetPath) ..\..\..\..\..\epoc32\tools\ecmt\$(TargetName).exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "EcmtGwUI - Win32 Release"
# Name "EcmtGwUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\EcmtGwUI.c
# End Source File
# Begin Source File

SOURCE=.\MainDlg.c
# End Source File
# Begin Source File

SOURCE=.\MainWnd.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\EcmtGwUI.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\BT.ico
# End Source File
# Begin Source File

SOURCE=.\res\ComPort.ico
# End Source File
# Begin Source File

SOURCE=.\res\EcmtGw.ico
# End Source File
# Begin Source File

SOURCE=.\res\EcmtGwRc.h
# End Source File
# Begin Source File

SOURCE=.\res\EcmtGwUI.rc
# End Source File
# Begin Source File

SOURCE=.\res\Empty.ico
# End Source File
# Begin Source File

SOURCE=.\res\green.bmp
# End Source File
# Begin Source File

SOURCE=.\res\grey.bmp
# End Source File
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\red.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Refresh.avi
# End Source File
# Begin Source File

SOURCE=.\res\Throbber.avi
# End Source File
# Begin Source File

SOURCE=.\res\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Unknown.ico
# End Source File
# Begin Source File

SOURCE=.\res\Wlan.ico
# End Source File
# End Group
# End Target
# End Project
