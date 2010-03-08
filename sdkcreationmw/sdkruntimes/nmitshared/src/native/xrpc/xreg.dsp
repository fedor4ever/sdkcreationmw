# Microsoft Developer Studio Project File - Name="xreg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=xreg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xreg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xreg.mak" CFG="xreg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xreg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "xreg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xreg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "build/xreg/release"
# PROP BASE Intermediate_Dir "build/xreg/release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build/xreg/release"
# PROP Intermediate_Dir "build/xreg/release"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "include" /I "xreg/include" /I "../slib/include" /I "../ucs2lib/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "USE_EXPAT" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "xreg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "build/xreg/debug"
# PROP BASE Intermediate_Dir "build/xreg/debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build/xreg/debug"
# PROP Intermediate_Dir "build/xreg/debug"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "include" /I "xreg/include" /I "../slib/include" /I "../ucs2lib/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "USE_EXPAT" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"build/xreg/debug/xregd.lib"

!ENDIF 

# Begin Target

# Name "xreg - Win32 Release"
# Name "xreg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\xreg\src\x_regapi.c
# End Source File
# Begin Source File

SOURCE=.\xreg\src\x_regcln.c
# End Source File
# Begin Source File

SOURCE=.\xreg\src\x_regsrv.c
# End Source File
# Begin Source File

SOURCE=.\xreg\src\x_regutl.c
# End Source File
# End Group
# Begin Group "Internal Headers"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\xreg\src\x_reg.h
# End Source File
# End Group
# Begin Group "Public Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xreg\include\xreg.h
# End Source File
# Begin Source File

SOURCE=.\xreg\include\xregapi.h
# End Source File
# End Group
# End Target
# End Project
