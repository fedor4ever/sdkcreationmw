# Microsoft Developer Studio Project File - Name="GwEngine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GwEngine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GwEngine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GwEngine.mak" CFG="GwEngine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GwEngine - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GwEngine - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GwEngine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build\Release"
# PROP Intermediate_Dir "build\Release"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "src/bt/include" /I "include" /I "../../../../NmitShared/src/native/slib/include" /I "../../../../NmitShared/src/native/wlib" /I "../../../../NmitShared/src/native/xrpc/include" /I "../../../../NmitShared/src/native/xrpc/xreg/include" /I "../../../NMIT/lib/include" /I "../../../EcmtServer/server/plugins/MidpDebug/inc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "GwEngine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build\Debug"
# PROP Intermediate_Dir "build\Debug"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "src/bt/include" /I "include" /I "../../../../NmitShared/src/native/slib/include" /I "../../../../NmitShared/src/native/wlib" /I "../../../../NmitShared/src/native/xrpc/include" /I "../../../../NmitShared/src/native/xrpc/xreg/include" /I "../../../NMIT/lib/include" /I "../../../EcmtServer/server/plugins/MidpDebug/inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "GwEngine - Win32 Release"
# Name "GwEngine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\GwBtSock.c
# End Source File
# Begin Source File

SOURCE=.\src\GwComPort.c
# End Source File
# Begin Source File

SOURCE=.\src\GwEngine.c
# End Source File
# Begin Source File

SOURCE=.\src\GwIoMp.c
# End Source File
# Begin Source File

SOURCE=.\src\GwIoPort.c
# End Source File
# Begin Source File

SOURCE=.\src\GwIoQue.c
# End Source File
# Begin Source File

SOURCE=.\src\GwIoSock.c
# End Source File
# Begin Source File

SOURCE=.\src\GwLanPort.c
# End Source File
# Begin Source File

SOURCE=.\src\GwPort.c
# End Source File
# Begin Source File

SOURCE=.\src\GwQueue.c
# End Source File
# Begin Source File

SOURCE=.\src\GwRead.c
# End Source File
# Begin Source File

SOURCE=.\src\GwSei.c
# End Source File
# Begin Source File

SOURCE=.\src\GwUtil.c
# End Source File
# Begin Source File

SOURCE=.\src\GwXrpc.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\GwEngine.h
# End Source File
# Begin Source File

SOURCE=.\include\GwXrpc.h
# End Source File
# End Group
# Begin Group "Private Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\EcmtGwP.h
# End Source File
# End Group
# End Target
# End Project
