# Microsoft Developer Studio Generated NMAKE File, Based on hcodec.dsp
!IF "$(CFG)" == ""
CFG=hcodec - Win32 Debug
!MESSAGE No configuration specified. Defaulting to hcodec - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "hcodec - Win32 Release" && "$(CFG)" != "hcodec - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hcodec.mak" CFG="hcodec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hcodec - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "hcodec - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "hcodec - Win32 Release"

OUTDIR=.\build/release
INTDIR=.\build/release
# Begin Custom Macros
OutDir=.\build/release
# End Custom Macros

ALL : "$(OUTDIR)\hcodec.lib"


CLEAN :
	-@erase "$(INTDIR)\hc_decoder.obj"
	-@erase "$(INTDIR)\hc_encoder.obj"
	-@erase "$(INTDIR)\hc_pages.obj"
	-@erase "$(INTDIR)\hc_util.obj"
	-@erase "$(INTDIR)\other_util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\hcodec.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "../include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "EXTERNAL_CU_FUNCTIONS" /Fp"$(INTDIR)\hcodec.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"build/hcodec.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\hcodec.lib" 
LIB32_OBJS= \
	"$(INTDIR)\hc_decoder.obj" \
	"$(INTDIR)\hc_encoder.obj" \
	"$(INTDIR)\hc_pages.obj" \
	"$(INTDIR)\hc_util.obj" \
	"$(INTDIR)\other_util.obj"

"$(OUTDIR)\hcodec.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hcodec - Win32 Debug"

OUTDIR=.\build/debug
INTDIR=.\build/debug
# Begin Custom Macros
OutDir=.\build/debug
# End Custom Macros

ALL : "$(OUTDIR)\hcodec.lib" ".\build\hcodec.bsc"


CLEAN :
	-@erase "$(INTDIR)\hc_decoder.obj"
	-@erase "$(INTDIR)\hc_decoder.sbr"
	-@erase "$(INTDIR)\hc_encoder.obj"
	-@erase "$(INTDIR)\hc_encoder.sbr"
	-@erase "$(INTDIR)\hc_pages.obj"
	-@erase "$(INTDIR)\hc_pages.sbr"
	-@erase "$(INTDIR)\hc_util.obj"
	-@erase "$(INTDIR)\hc_util.sbr"
	-@erase "$(INTDIR)\other_util.obj"
	-@erase "$(INTDIR)\other_util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\hcodec.lib"
	-@erase ".\build\hcodec.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "../include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "EXTERNAL_CU_FUNCTIONS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hcodec.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"build/hcodec.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\hc_decoder.sbr" \
	"$(INTDIR)\hc_encoder.sbr" \
	"$(INTDIR)\hc_pages.sbr" \
	"$(INTDIR)\hc_util.sbr" \
	"$(INTDIR)\other_util.sbr"

".\build\hcodec.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\hcodec.lib" 
LIB32_OBJS= \
	"$(INTDIR)\hc_decoder.obj" \
	"$(INTDIR)\hc_encoder.obj" \
	"$(INTDIR)\hc_pages.obj" \
	"$(INTDIR)\hc_util.obj" \
	"$(INTDIR)\other_util.obj"

"$(OUTDIR)\hcodec.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("hcodec.dep")
!INCLUDE "hcodec.dep"
!ELSE 
!MESSAGE Warning: cannot find "hcodec.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "hcodec - Win32 Release" || "$(CFG)" == "hcodec - Win32 Debug"
SOURCE=..\src\hc_decoder.c

!IF  "$(CFG)" == "hcodec - Win32 Release"


"$(INTDIR)\hc_decoder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "hcodec - Win32 Debug"


"$(INTDIR)\hc_decoder.obj"	"$(INTDIR)\hc_decoder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\hc_encoder.c

!IF  "$(CFG)" == "hcodec - Win32 Release"


"$(INTDIR)\hc_encoder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "hcodec - Win32 Debug"


"$(INTDIR)\hc_encoder.obj"	"$(INTDIR)\hc_encoder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\hc_pages.c

!IF  "$(CFG)" == "hcodec - Win32 Release"


"$(INTDIR)\hc_pages.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "hcodec - Win32 Debug"


"$(INTDIR)\hc_pages.obj"	"$(INTDIR)\hc_pages.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\hc_util.c

!IF  "$(CFG)" == "hcodec - Win32 Release"


"$(INTDIR)\hc_util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "hcodec - Win32 Debug"


"$(INTDIR)\hc_util.obj"	"$(INTDIR)\hc_util.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\other_util.c

!IF  "$(CFG)" == "hcodec - Win32 Release"


"$(INTDIR)\other_util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "hcodec - Win32 Debug"


"$(INTDIR)\other_util.obj"	"$(INTDIR)\other_util.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

