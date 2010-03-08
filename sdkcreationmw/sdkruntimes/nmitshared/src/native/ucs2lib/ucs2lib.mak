# Microsoft Developer Studio Generated NMAKE File, Based on ucs2lib.dsp
!IF "$(CFG)" == ""
CFG=ucs2lib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ucs2lib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ucs2lib - Win32 Release" && "$(CFG)" != "ucs2lib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ucs2lib.mak" CFG="ucs2lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ucs2lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ucs2lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ucs2lib - Win32 Release"

OUTDIR=.\build/release
INTDIR=.\build/release
# Begin Custom Macros
OutDir=.\build/release
# End Custom Macros

ALL : "$(OUTDIR)\ucs2lib.lib"


CLEAN :
	-@erase "$(INTDIR)\ucs2.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ucs2lib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "include" /I "$(NMIT_SHARED_ROOT)/src/native/slib/include" /D "NDEBUG" /D "WIN32" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ucs2lib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\ucs2lib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ucs2.obj"

"$(OUTDIR)\ucs2lib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ucs2lib - Win32 Debug"

OUTDIR=.\build/debug
INTDIR=.\build/debug
# Begin Custom Macros
OutDir=.\build/debug
# End Custom Macros

ALL : "$(OUTDIR)\ucs2libd.lib" "$(OUTDIR)\ucs2lib.bsc"


CLEAN :
	-@erase "$(INTDIR)\ucs2.obj"
	-@erase "$(INTDIR)\ucs2.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ucs2lib.bsc"
	-@erase "$(OUTDIR)\ucs2libd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "include" /I "$(NMIT_SHARED_ROOT)/src/native/slib/include" /D "_DEBUG" /D "WIN32" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ucs2lib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ucs2.sbr"

"$(OUTDIR)\ucs2lib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\ucs2libd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ucs2.obj"

"$(OUTDIR)\ucs2libd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ucs2lib.dep")
!INCLUDE "ucs2lib.dep"
!ELSE 
!MESSAGE Warning: cannot find "ucs2lib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ucs2lib - Win32 Release" || "$(CFG)" == "ucs2lib - Win32 Debug"
SOURCE=.\src\ucs2.c

!IF  "$(CFG)" == "ucs2lib - Win32 Release"


"$(INTDIR)\ucs2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ucs2lib - Win32 Debug"


"$(INTDIR)\ucs2.obj"	"$(INTDIR)\ucs2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

