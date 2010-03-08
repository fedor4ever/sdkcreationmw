# Microsoft Developer Studio Project File - Name="jnilib_mt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=jnilib_mt - Win32 DLL Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jnilib_mt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jnilib_mt.mak" CFG="jnilib_mt - Win32 DLL Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jnilib_mt - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jnilib_mt - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "jnilib_mt - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "jnilib_mt - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jnilib_mt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "build/mt/release"
# PROP BASE Intermediate_Dir "build/mt/release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build/mt/release"
# PROP Intermediate_Dir "build/mt/release"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../slib/include" /I "./src" /I "./include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /FR /YX /FD /EHa /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "jnilib_mt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "build/mt/debug"
# PROP BASE Intermediate_Dir "build/mt/debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build/mt/debug"
# PROP Intermediate_Dir "build/mt/debug"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "../slib/include" /I "./src" /I "./include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /FR /YX /FD /EHa /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"build/mt/debug/jnilib_mtd.lib"

!ELSEIF  "$(CFG)" == "jnilib_mt - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "build/mt/debug"
# PROP BASE Intermediate_Dir "build/mt/debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build/mt/debug"
# PROP Intermediate_Dir "build/mt/debug"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I "../slib/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /EHa /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "../slib/include" /I "./src" /I "./include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /FR /YX /FD /EHa /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"build/mt/debug/jnilib_mtd.lib"
# ADD LIB32 /nologo /out:"build/mt/debug/jnilib_mtd.lib"

!ELSEIF  "$(CFG)" == "jnilib_mt - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "build/mt/release"
# PROP BASE Intermediate_Dir "build/mt/release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build/mt/release"
# PROP Intermediate_Dir "build/mt/release"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../slib/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /EHa /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../slib/include" /I "./src" /I "./include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /FR /YX /FD /EHa /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "jnilib_mt - Win32 Release"
# Name "jnilib_mt - Win32 Debug"
# Name "jnilib_mt - Win32 DLL Debug"
# Name "jnilib_mt - Win32 DLL Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\jni_call.c
# End Source File
# Begin Source File

SOURCE=.\src\jni_data.c
# End Source File
# Begin Source File

SOURCE=.\src\jni_new.c
# End Source File
# Begin Source File

SOURCE=.\src\jni_str.c
# End Source File
# Begin Source File

SOURCE=.\src\jni_util.c
# End Source File
# Begin Source File

SOURCE=.\src\jni_win32.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\jni_lib.h
# End Source File
# End Group
# Begin Group "Private Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\jni_libp.h
# End Source File
# End Group
# End Target
# End Project
