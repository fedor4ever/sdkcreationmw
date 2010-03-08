# Microsoft Developer Studio Project File - Name="wmlsenc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wmlsenc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wmlsenc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wmlsenc.mak" CFG="wmlsenc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wmlsenc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wmlsenc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wmlsenc - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(RAINBOW_HOME)/rb_wps/server/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "STANDARD_LIBRARIES" /D "CRYPTO_LIBRARIES" /D yyerror=WMLS_yyerror /D alloca=malloc /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "$(RAINBOW_HOME)/rb_wps/server/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "STANDARD_LIBRARIES" /D "CRYPTO_LIBRARIES" /D yyerror=WMLS_yyerror /D alloca=malloc /FR /YX /FD /GZ /c
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

# Name "wmlsenc - Win32 Release"
# Name "wmlsenc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\bytecode.c
# End Source File
# Begin Source File

SOURCE=.\src\codenode.c
# End Source File
# Begin Source File

SOURCE=.\src\conspool.c
# End Source File
# Begin Source File

SOURCE=.\src\funcpool.c
# End Source File
# Begin Source File

SOURCE=.\src\lex.c
# End Source File
# Begin Source File

SOURCE=.\src\library.c
# End Source File
# Begin Source File

SOURCE=.\src\main.c
# End Source File
# Begin Source File

SOURCE=.\src\memmana.c
# End Source File
# Begin Source File

SOURCE=.\src\pragpool.c
# End Source File
# Begin Source File

SOURCE=.\src\symtable.c
# End Source File
# Begin Source File

SOURCE=.\src\wmlsyacc.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\bytecode.h
# End Source File
# Begin Source File

SOURCE=.\src\codenode.h
# End Source File
# Begin Source File

SOURCE=.\src\common.h
# End Source File
# Begin Source File

SOURCE=.\src\conspool.h
# End Source File
# Begin Source File

SOURCE=.\src\funcpool.h
# End Source File
# Begin Source File

SOURCE=.\src\lex.h
# End Source File
# Begin Source File

SOURCE=.\src\library.h
# End Source File
# Begin Source File

SOURCE=.\src\main.h
# End Source File
# Begin Source File

SOURCE=.\src\memmana.h
# End Source File
# Begin Source File

SOURCE=.\src\opcodes.h
# End Source File
# Begin Source File

SOURCE=.\src\pragpool.h
# End Source File
# Begin Source File

SOURCE=.\src\symtable.h
# End Source File
# Begin Source File

SOURCE=.\src\wmlsyacc.h
# End Source File
# End Group
# End Target
# End Project
