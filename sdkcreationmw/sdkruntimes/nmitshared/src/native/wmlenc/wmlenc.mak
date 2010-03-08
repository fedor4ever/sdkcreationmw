# Microsoft Developer Studio Generated NMAKE File, Based on wmlenc.dsp
!IF "$(CFG)" == ""
CFG=wmlenc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to wmlenc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "wmlenc - Win32 Release" && "$(CFG)" != "wmlenc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "wmlenc - Win32 Release"

OUTDIR=.\build/release
INTDIR=.\build/release
# Begin Custom Macros
OutDir=.\build/release
# End Custom Macros

ALL : "$(OUTDIR)\wmlenc.lib"


CLEAN :
	-@erase "$(INTDIR)\chenc.obj"
	-@erase "$(INTDIR)\chmap.obj"
	-@erase "$(INTDIR)\chtables.obj"
	-@erase "$(INTDIR)\codeload.obj"
	-@erase "$(INTDIR)\mibenum.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wmlarray.obj"
	-@erase "$(INTDIR)\wmlbin.obj"
	-@erase "$(INTDIR)\wmlhndl.obj"
	-@erase "$(INTDIR)\wmllog.obj"
	-@erase "$(INTDIR)\wmlmain.obj"
	-@erase "$(INTDIR)\wmlsegm.obj"
	-@erase "$(INTDIR)\wmlstruc.obj"
	-@erase "$(INTDIR)\wmlvalid.obj"
	-@erase "$(INTDIR)\wmlvar.obj"
	-@erase "$(INTDIR)\xmlbuf.obj"
	-@erase "$(INTDIR)\xmlcomm.obj"
	-@erase "$(INTDIR)\xmlconst.obj"
	-@erase "$(INTDIR)\xmlcp.obj"
	-@erase "$(INTDIR)\xmlctyp.obj"
	-@erase "$(INTDIR)\xmllex.obj"
	-@erase "$(INTDIR)\xmlmem.obj"
	-@erase "$(INTDIR)\xmlmsg.obj"
	-@erase "$(INTDIR)\xmlpars.obj"
	-@erase "$(INTDIR)\xmlpool.obj"
	-@erase "$(INTDIR)\xmlrules.obj"
	-@erase "$(INTDIR)\xmlstr.obj"
	-@erase "$(INTDIR)\xmlstruc.obj"
	-@erase "$(INTDIR)\xmlsymt.obj"
	-@erase "$(INTDIR)\xmlvc.obj"
	-@erase "$(OUTDIR)\wmlenc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "$(RAINBOW_HOME)/rb_wps/server/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "WSL_NO_LOG_LOG" /Fp"$(INTDIR)\wmlenc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wmlenc.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\wmlenc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\chenc.obj" \
	"$(INTDIR)\chmap.obj" \
	"$(INTDIR)\chtables.obj" \
	"$(INTDIR)\codeload.obj" \
	"$(INTDIR)\mibenum.obj" \
	"$(INTDIR)\wmlarray.obj" \
	"$(INTDIR)\wmlbin.obj" \
	"$(INTDIR)\wmlhndl.obj" \
	"$(INTDIR)\wmllog.obj" \
	"$(INTDIR)\wmlmain.obj" \
	"$(INTDIR)\wmlsegm.obj" \
	"$(INTDIR)\wmlstruc.obj" \
	"$(INTDIR)\wmlvalid.obj" \
	"$(INTDIR)\wmlvar.obj" \
	"$(INTDIR)\xmlbuf.obj" \
	"$(INTDIR)\xmlcomm.obj" \
	"$(INTDIR)\xmlconst.obj" \
	"$(INTDIR)\xmlcp.obj" \
	"$(INTDIR)\xmlctyp.obj" \
	"$(INTDIR)\xmllex.obj" \
	"$(INTDIR)\xmlmem.obj" \
	"$(INTDIR)\xmlmsg.obj" \
	"$(INTDIR)\xmlpars.obj" \
	"$(INTDIR)\xmlpool.obj" \
	"$(INTDIR)\xmlrules.obj" \
	"$(INTDIR)\xmlstr.obj" \
	"$(INTDIR)\xmlstruc.obj" \
	"$(INTDIR)\xmlsymt.obj" \
	"$(INTDIR)\xmlvc.obj"

"$(OUTDIR)\wmlenc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"

OUTDIR=.\build/debug
INTDIR=.\build/debug
# Begin Custom Macros
OutDir=.\build/debug
# End Custom Macros

ALL : "$(OUTDIR)\wmlenc.lib" "$(OUTDIR)\wmlenc.bsc"


CLEAN :
	-@erase "$(INTDIR)\chenc.obj"
	-@erase "$(INTDIR)\chenc.sbr"
	-@erase "$(INTDIR)\chmap.obj"
	-@erase "$(INTDIR)\chmap.sbr"
	-@erase "$(INTDIR)\chtables.obj"
	-@erase "$(INTDIR)\chtables.sbr"
	-@erase "$(INTDIR)\codeload.obj"
	-@erase "$(INTDIR)\codeload.sbr"
	-@erase "$(INTDIR)\mibenum.obj"
	-@erase "$(INTDIR)\mibenum.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wmlarray.obj"
	-@erase "$(INTDIR)\wmlarray.sbr"
	-@erase "$(INTDIR)\wmlbin.obj"
	-@erase "$(INTDIR)\wmlbin.sbr"
	-@erase "$(INTDIR)\wmlhndl.obj"
	-@erase "$(INTDIR)\wmlhndl.sbr"
	-@erase "$(INTDIR)\wmllog.obj"
	-@erase "$(INTDIR)\wmllog.sbr"
	-@erase "$(INTDIR)\wmlmain.obj"
	-@erase "$(INTDIR)\wmlmain.sbr"
	-@erase "$(INTDIR)\wmlsegm.obj"
	-@erase "$(INTDIR)\wmlsegm.sbr"
	-@erase "$(INTDIR)\wmlstruc.obj"
	-@erase "$(INTDIR)\wmlstruc.sbr"
	-@erase "$(INTDIR)\wmlvalid.obj"
	-@erase "$(INTDIR)\wmlvalid.sbr"
	-@erase "$(INTDIR)\wmlvar.obj"
	-@erase "$(INTDIR)\wmlvar.sbr"
	-@erase "$(INTDIR)\xmlbuf.obj"
	-@erase "$(INTDIR)\xmlbuf.sbr"
	-@erase "$(INTDIR)\xmlcomm.obj"
	-@erase "$(INTDIR)\xmlcomm.sbr"
	-@erase "$(INTDIR)\xmlconst.obj"
	-@erase "$(INTDIR)\xmlconst.sbr"
	-@erase "$(INTDIR)\xmlcp.obj"
	-@erase "$(INTDIR)\xmlcp.sbr"
	-@erase "$(INTDIR)\xmlctyp.obj"
	-@erase "$(INTDIR)\xmlctyp.sbr"
	-@erase "$(INTDIR)\xmllex.obj"
	-@erase "$(INTDIR)\xmllex.sbr"
	-@erase "$(INTDIR)\xmlmem.obj"
	-@erase "$(INTDIR)\xmlmem.sbr"
	-@erase "$(INTDIR)\xmlmsg.obj"
	-@erase "$(INTDIR)\xmlmsg.sbr"
	-@erase "$(INTDIR)\xmlpars.obj"
	-@erase "$(INTDIR)\xmlpars.sbr"
	-@erase "$(INTDIR)\xmlpool.obj"
	-@erase "$(INTDIR)\xmlpool.sbr"
	-@erase "$(INTDIR)\xmlrules.obj"
	-@erase "$(INTDIR)\xmlrules.sbr"
	-@erase "$(INTDIR)\xmlstr.obj"
	-@erase "$(INTDIR)\xmlstr.sbr"
	-@erase "$(INTDIR)\xmlstruc.obj"
	-@erase "$(INTDIR)\xmlstruc.sbr"
	-@erase "$(INTDIR)\xmlsymt.obj"
	-@erase "$(INTDIR)\xmlsymt.sbr"
	-@erase "$(INTDIR)\xmlvc.obj"
	-@erase "$(INTDIR)\xmlvc.sbr"
	-@erase "$(OUTDIR)\wmlenc.bsc"
	-@erase "$(OUTDIR)\wmlenc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "$(RAINBOW_HOME)/rb_wps/server/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "WSL_NO_LOG_LOG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\wmlenc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wmlenc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\chenc.sbr" \
	"$(INTDIR)\chmap.sbr" \
	"$(INTDIR)\chtables.sbr" \
	"$(INTDIR)\codeload.sbr" \
	"$(INTDIR)\mibenum.sbr" \
	"$(INTDIR)\wmlarray.sbr" \
	"$(INTDIR)\wmlbin.sbr" \
	"$(INTDIR)\wmlhndl.sbr" \
	"$(INTDIR)\wmllog.sbr" \
	"$(INTDIR)\wmlmain.sbr" \
	"$(INTDIR)\wmlsegm.sbr" \
	"$(INTDIR)\wmlstruc.sbr" \
	"$(INTDIR)\wmlvalid.sbr" \
	"$(INTDIR)\wmlvar.sbr" \
	"$(INTDIR)\xmlbuf.sbr" \
	"$(INTDIR)\xmlcomm.sbr" \
	"$(INTDIR)\xmlconst.sbr" \
	"$(INTDIR)\xmlcp.sbr" \
	"$(INTDIR)\xmlctyp.sbr" \
	"$(INTDIR)\xmllex.sbr" \
	"$(INTDIR)\xmlmem.sbr" \
	"$(INTDIR)\xmlmsg.sbr" \
	"$(INTDIR)\xmlpars.sbr" \
	"$(INTDIR)\xmlpool.sbr" \
	"$(INTDIR)\xmlrules.sbr" \
	"$(INTDIR)\xmlstr.sbr" \
	"$(INTDIR)\xmlstruc.sbr" \
	"$(INTDIR)\xmlsymt.sbr" \
	"$(INTDIR)\xmlvc.sbr"

"$(OUTDIR)\wmlenc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\wmlenc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\chenc.obj" \
	"$(INTDIR)\chmap.obj" \
	"$(INTDIR)\chtables.obj" \
	"$(INTDIR)\codeload.obj" \
	"$(INTDIR)\mibenum.obj" \
	"$(INTDIR)\wmlarray.obj" \
	"$(INTDIR)\wmlbin.obj" \
	"$(INTDIR)\wmlhndl.obj" \
	"$(INTDIR)\wmllog.obj" \
	"$(INTDIR)\wmlmain.obj" \
	"$(INTDIR)\wmlsegm.obj" \
	"$(INTDIR)\wmlstruc.obj" \
	"$(INTDIR)\wmlvalid.obj" \
	"$(INTDIR)\wmlvar.obj" \
	"$(INTDIR)\xmlbuf.obj" \
	"$(INTDIR)\xmlcomm.obj" \
	"$(INTDIR)\xmlconst.obj" \
	"$(INTDIR)\xmlcp.obj" \
	"$(INTDIR)\xmlctyp.obj" \
	"$(INTDIR)\xmllex.obj" \
	"$(INTDIR)\xmlmem.obj" \
	"$(INTDIR)\xmlmsg.obj" \
	"$(INTDIR)\xmlpars.obj" \
	"$(INTDIR)\xmlpool.obj" \
	"$(INTDIR)\xmlrules.obj" \
	"$(INTDIR)\xmlstr.obj" \
	"$(INTDIR)\xmlstruc.obj" \
	"$(INTDIR)\xmlsymt.obj" \
	"$(INTDIR)\xmlvc.obj"

"$(OUTDIR)\wmlenc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("wmlenc.dep")
!INCLUDE "wmlenc.dep"
!ELSE 
!MESSAGE Warning: cannot find "wmlenc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "wmlenc - Win32 Release" || "$(CFG)" == "wmlenc - Win32 Debug"
SOURCE=.\src\chenc.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\chenc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\chenc.obj"	"$(INTDIR)\chenc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\chmap.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\chmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\chmap.obj"	"$(INTDIR)\chmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\chtables.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\chtables.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\chtables.obj"	"$(INTDIR)\chtables.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\codeload.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\codeload.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\codeload.obj"	"$(INTDIR)\codeload.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\mibenum.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\mibenum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\mibenum.obj"	"$(INTDIR)\mibenum.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\wmlarray.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\wmlarray.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\wmlarray.obj"	"$(INTDIR)\wmlarray.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\wmlbin.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\wmlbin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\wmlbin.obj"	"$(INTDIR)\wmlbin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\wmlhndl.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\wmlhndl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\wmlhndl.obj"	"$(INTDIR)\wmlhndl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\wmllog.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\wmllog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\wmllog.obj"	"$(INTDIR)\wmllog.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\wmlmain.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\wmlmain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\wmlmain.obj"	"$(INTDIR)\wmlmain.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\wmlsegm.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\wmlsegm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\wmlsegm.obj"	"$(INTDIR)\wmlsegm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\wmlstruc.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\wmlstruc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\wmlstruc.obj"	"$(INTDIR)\wmlstruc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\wmlvalid.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\wmlvalid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\wmlvalid.obj"	"$(INTDIR)\wmlvalid.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\wmlvar.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\wmlvar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\wmlvar.obj"	"$(INTDIR)\wmlvar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlbuf.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlbuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlbuf.obj"	"$(INTDIR)\xmlbuf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlcomm.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlcomm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlcomm.obj"	"$(INTDIR)\xmlcomm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlconst.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlconst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlconst.obj"	"$(INTDIR)\xmlconst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlcp.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlcp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlcp.obj"	"$(INTDIR)\xmlcp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlctyp.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlctyp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlctyp.obj"	"$(INTDIR)\xmlctyp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmllex.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmllex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmllex.obj"	"$(INTDIR)\xmllex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlmem.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlmem.obj"	"$(INTDIR)\xmlmem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlmsg.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlmsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlmsg.obj"	"$(INTDIR)\xmlmsg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlpars.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlpars.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlpars.obj"	"$(INTDIR)\xmlpars.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlpool.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlpool.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlpool.obj"	"$(INTDIR)\xmlpool.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlrules.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlrules.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlrules.obj"	"$(INTDIR)\xmlrules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlstr.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlstr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlstr.obj"	"$(INTDIR)\xmlstr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlstruc.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlstruc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlstruc.obj"	"$(INTDIR)\xmlstruc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlsymt.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlsymt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlsymt.obj"	"$(INTDIR)\xmlsymt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\xmlvc.c

!IF  "$(CFG)" == "wmlenc - Win32 Release"


"$(INTDIR)\xmlvc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlenc - Win32 Debug"


"$(INTDIR)\xmlvc.obj"	"$(INTDIR)\xmlvc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

