# Microsoft Developer Studio Generated NMAKE File, Based on xreg.dsp
!IF "$(CFG)" == ""
CFG=xreg - Win32 Debug
!MESSAGE No configuration specified. Defaulting to xreg - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "xreg - Win32 Release" && "$(CFG)" != "xreg - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "xreg - Win32 Release"

OUTDIR=.\build/xreg/release
INTDIR=.\build/xreg/release
# Begin Custom Macros
OutDir=.\build/xreg/release
# End Custom Macros

ALL : "$(OUTDIR)\xreg.lib" "$(OUTDIR)\xreg.bsc"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\x_regapi.obj"
	-@erase "$(INTDIR)\x_regapi.sbr"
	-@erase "$(INTDIR)\x_regcln.obj"
	-@erase "$(INTDIR)\x_regcln.sbr"
	-@erase "$(INTDIR)\x_regsrv.obj"
	-@erase "$(INTDIR)\x_regsrv.sbr"
	-@erase "$(INTDIR)\x_regutl.obj"
	-@erase "$(INTDIR)\x_regutl.sbr"
	-@erase "$(OUTDIR)\xreg.bsc"
	-@erase "$(OUTDIR)\xreg.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "./include" /I "xreg/include" /I "$(NMIT_SHARED_ROOT)/src/native/slib/include" /I "$(NMIT_SHARED_ROOT)/src/native/zlib" /I "include" /I "../ucs2lib/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "USE_EXPAT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\xreg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xreg.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\x_regapi.sbr" \
	"$(INTDIR)\x_regcln.sbr" \
	"$(INTDIR)\x_regsrv.sbr" \
	"$(INTDIR)\x_regutl.sbr"

"$(OUTDIR)\xreg.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\xreg.lib" 
LIB32_OBJS= \
	"$(INTDIR)\x_regapi.obj" \
	"$(INTDIR)\x_regcln.obj" \
	"$(INTDIR)\x_regsrv.obj" \
	"$(INTDIR)\x_regutl.obj"

"$(OUTDIR)\xreg.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xreg - Win32 Debug"

OUTDIR=.\build/xreg/debug
INTDIR=.\build/xreg/debug
# Begin Custom Macros
OutDir=.\build/xreg/debug
# End Custom Macros

ALL : "$(OUTDIR)\xregd.lib" "$(OUTDIR)\xreg.bsc"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\x_regapi.obj"
	-@erase "$(INTDIR)\x_regapi.sbr"
	-@erase "$(INTDIR)\x_regcln.obj"
	-@erase "$(INTDIR)\x_regcln.sbr"
	-@erase "$(INTDIR)\x_regsrv.obj"
	-@erase "$(INTDIR)\x_regsrv.sbr"
	-@erase "$(INTDIR)\x_regutl.obj"
	-@erase "$(INTDIR)\x_regutl.sbr"
	-@erase "$(OUTDIR)\xreg.bsc"
	-@erase "$(OUTDIR)\xregd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "./include" /I "xreg/include" /I "$(NMIT_SHARED_ROOT)/src/native/slib/include" /I "$(NMIT_SHARED_ROOT)/src/native/zlib" /I "include" /I "../ucs2lib/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "USE_EXPAT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\xreg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xreg.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\x_regapi.sbr" \
	"$(INTDIR)\x_regcln.sbr" \
	"$(INTDIR)\x_regsrv.sbr" \
	"$(INTDIR)\x_regutl.sbr"

"$(OUTDIR)\xreg.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\xregd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\x_regapi.obj" \
	"$(INTDIR)\x_regcln.obj" \
	"$(INTDIR)\x_regsrv.obj" \
	"$(INTDIR)\x_regutl.obj"

"$(OUTDIR)\xregd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("xreg.dep")
!INCLUDE "xreg.dep"
!ELSE 
!MESSAGE Warning: cannot find "xreg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "xreg - Win32 Release" || "$(CFG)" == "xreg - Win32 Debug"
SOURCE=.\xreg\src\x_regapi.c

"$(INTDIR)\x_regapi.obj"	"$(INTDIR)\x_regapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\xreg\src\x_regcln.c

"$(INTDIR)\x_regcln.obj"	"$(INTDIR)\x_regcln.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\xreg\src\x_regsrv.c

"$(INTDIR)\x_regsrv.obj"	"$(INTDIR)\x_regsrv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\xreg\src\x_regutl.c

"$(INTDIR)\x_regutl.obj"	"$(INTDIR)\x_regutl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

