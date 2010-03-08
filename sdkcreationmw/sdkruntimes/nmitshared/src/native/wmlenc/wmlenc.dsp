# Microsoft Developer Studio Project File - Name="wmlenc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wmlenc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wmlenc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wmlenc.mak" CFG="wmlenc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wmlenc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wmlenc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wmlenc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "build/release"
# PROP BASE Intermediate_Dir "build/release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build/release"
# PROP Intermediate_Dir "build/release"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(RAINBOW_HOME)/rb_wps/server/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "WSL_NO_LOG_LOG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "build/debug"
# PROP BASE Intermediate_Dir "build/debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build/debug"
# PROP Intermediate_Dir "build/debug"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "$(RAINBOW_HOME)/rb_wps/server/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "WSL_NO_LOG_LOG" /FR /YX /FD /GZ /c
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

# Name "wmlenc - Win32 Release"
# Name "wmlenc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\chenc.c
# End Source File
# Begin Source File

SOURCE=.\src\chmap.c
# End Source File
# Begin Source File

SOURCE=.\src\chtables.c
# End Source File
# Begin Source File

SOURCE=.\src\codeload.c
# End Source File
# Begin Source File

SOURCE=.\src\mibenum.c
# End Source File
# Begin Source File

SOURCE=.\src\wmlarray.c
# End Source File
# Begin Source File

SOURCE=.\src\wmlbin.c
# End Source File
# Begin Source File

SOURCE=.\src\wmlhndl.c
# End Source File
# Begin Source File

SOURCE=.\src\wmllog.c
# End Source File
# Begin Source File

SOURCE=.\src\wmlmain.c
# End Source File
# Begin Source File

SOURCE=.\src\wmlsegm.c
# End Source File
# Begin Source File

SOURCE=.\src\wmlstruc.c
# End Source File
# Begin Source File

SOURCE=.\src\wmlvalid.c
# End Source File
# Begin Source File

SOURCE=.\src\wmlvar.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlbuf.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlcomm.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlconst.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlcp.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlctyp.c
# End Source File
# Begin Source File

SOURCE=.\src\xmllex.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlmem.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlmsg.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlpars.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlpool.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlrules.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlstr.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlstruc.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlsymt.c
# End Source File
# Begin Source File

SOURCE=.\src\xmlvc.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\chenc.h
# End Source File
# Begin Source File

SOURCE=.\src\chmap.h
# End Source File
# Begin Source File

SOURCE=.\src\wmlarray.h
# End Source File
# Begin Source File

SOURCE=.\src\wmlmsg.h
# End Source File
# Begin Source File

SOURCE=.\src\wmlvalid.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlapi.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlbuf.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlconst.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlctyp.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlhost.h
# End Source File
# Begin Source File

SOURCE=.\src\xmllex.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlmsg.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlpars.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlpool.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlrules.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlstr.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlstruc.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlsymt.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlvc.h
# End Source File
# End Group
# Begin Group "Config Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\co10dtd.h
# End Source File
# Begin Source File

SOURCE=.\include\co10tok.h
# End Source File
# Begin Source File

SOURCE=.\include\si10dtd.h
# End Source File
# Begin Source File

SOURCE=.\include\si10tok.h
# End Source File
# Begin Source File

SOURCE=.\include\sl10dtd.h
# End Source File
# Begin Source File

SOURCE=.\include\sl10tok.h
# End Source File
# Begin Source File

SOURCE=.\include\wml10dtd.h
# End Source File
# Begin Source File

SOURCE=.\include\wml10tok.h
# End Source File
# Begin Source File

SOURCE=.\include\wml11dtd.h
# End Source File
# Begin Source File

SOURCE=.\include\wml11tok.h
# End Source File
# Begin Source File

SOURCE=.\include\wml12dtd.h
# End Source File
# Begin Source File

SOURCE=.\include\wml12tok.h
# End Source File
# Begin Source File

SOURCE=.\include\wml13dtd.h
# End Source File
# Begin Source File

SOURCE=.\include\wml13tok.h
# End Source File
# End Group
# End Target
# End Project
