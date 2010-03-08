# Microsoft Developer Studio Generated NMAKE File, Based on xrpc.dsp
!IF "$(CFG)" == ""
CFG=xrpc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to xrpc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "xrpc - Win32 Release" && "$(CFG)" != "xrpc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xrpc.mak" CFG="xrpc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xrpc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "xrpc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "xrpc - Win32 Release"

OUTDIR=.\build/xrpc/release
INTDIR=.\build/xrpc/release
# Begin Custom Macros
OutDir=.\build/xrpc/release
# End Custom Macros

ALL : "$(OUTDIR)\xrpc.lib" "$(OUTDIR)\xrpc.bsc"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\x_client.obj"
	-@erase "$(INTDIR)\x_client.sbr"
	-@erase "$(INTDIR)\x_core.obj"
	-@erase "$(INTDIR)\x_core.sbr"
	-@erase "$(INTDIR)\x_data.obj"
	-@erase "$(INTDIR)\x_data.sbr"
	-@erase "$(INTDIR)\x_expat.obj"
	-@erase "$(INTDIR)\x_expat.sbr"
	-@erase "$(INTDIR)\x_libxml.obj"
	-@erase "$(INTDIR)\x_libxml.sbr"
	-@erase "$(INTDIR)\x_parse.obj"
	-@erase "$(INTDIR)\x_parse.sbr"
	-@erase "$(INTDIR)\x_server.obj"
	-@erase "$(INTDIR)\x_server.sbr"
	-@erase "$(INTDIR)\x_util.obj"
	-@erase "$(INTDIR)\x_util.sbr"
	-@erase "$(OUTDIR)\xrpc.bsc"
	-@erase "$(OUTDIR)\xrpc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "$(NMIT_SHARED_ROOT)/src/native/slib/include" /I "$(NMIT_SHARED_ROOT)/src/native/zlib" /I "include" /I "../ucs2lib/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "USE_EXPAT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\xrpc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xrpc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\x_client.sbr" \
	"$(INTDIR)\x_core.sbr" \
	"$(INTDIR)\x_data.sbr" \
	"$(INTDIR)\x_expat.sbr" \
	"$(INTDIR)\x_libxml.sbr" \
	"$(INTDIR)\x_parse.sbr" \
	"$(INTDIR)\x_server.sbr" \
	"$(INTDIR)\x_util.sbr"

"$(OUTDIR)\xrpc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\xrpc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\x_client.obj" \
	"$(INTDIR)\x_core.obj" \
	"$(INTDIR)\x_data.obj" \
	"$(INTDIR)\x_expat.obj" \
	"$(INTDIR)\x_libxml.obj" \
	"$(INTDIR)\x_parse.obj" \
	"$(INTDIR)\x_server.obj" \
	"$(INTDIR)\x_util.obj"

"$(OUTDIR)\xrpc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xrpc - Win32 Debug"

OUTDIR=.\build/xrpc/debug
INTDIR=.\build/xrpc/debug
# Begin Custom Macros
OutDir=.\build/xrpc/debug
# End Custom Macros

ALL : "$(OUTDIR)\xrpcd.lib" "$(OUTDIR)\xrpc.bsc"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\x_client.obj"
	-@erase "$(INTDIR)\x_client.sbr"
	-@erase "$(INTDIR)\x_core.obj"
	-@erase "$(INTDIR)\x_core.sbr"
	-@erase "$(INTDIR)\x_data.obj"
	-@erase "$(INTDIR)\x_data.sbr"
	-@erase "$(INTDIR)\x_expat.obj"
	-@erase "$(INTDIR)\x_expat.sbr"
	-@erase "$(INTDIR)\x_libxml.obj"
	-@erase "$(INTDIR)\x_libxml.sbr"
	-@erase "$(INTDIR)\x_parse.obj"
	-@erase "$(INTDIR)\x_parse.sbr"
	-@erase "$(INTDIR)\x_server.obj"
	-@erase "$(INTDIR)\x_server.sbr"
	-@erase "$(INTDIR)\x_util.obj"
	-@erase "$(INTDIR)\x_util.sbr"
	-@erase "$(OUTDIR)\xrpc.bsc"
	-@erase "$(OUTDIR)\xrpcd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "$(NMIT_SHARED_ROOT)/src/native/slib/include" /I "$(NMIT_SHARED_ROOT)/src/native/zlib" /I "include" /I "../ucs2lib/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "USE_EXPAT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\xrpc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xrpc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\x_client.sbr" \
	"$(INTDIR)\x_core.sbr" \
	"$(INTDIR)\x_data.sbr" \
	"$(INTDIR)\x_expat.sbr" \
	"$(INTDIR)\x_libxml.sbr" \
	"$(INTDIR)\x_parse.sbr" \
	"$(INTDIR)\x_server.sbr" \
	"$(INTDIR)\x_util.sbr"

"$(OUTDIR)\xrpc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\xrpcd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\x_client.obj" \
	"$(INTDIR)\x_core.obj" \
	"$(INTDIR)\x_data.obj" \
	"$(INTDIR)\x_expat.obj" \
	"$(INTDIR)\x_libxml.obj" \
	"$(INTDIR)\x_parse.obj" \
	"$(INTDIR)\x_server.obj" \
	"$(INTDIR)\x_util.obj"

"$(OUTDIR)\xrpcd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("xrpc.dep")
!INCLUDE "xrpc.dep"
!ELSE 
!MESSAGE Warning: cannot find "xrpc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "xrpc - Win32 Release" || "$(CFG)" == "xrpc - Win32 Debug"
SOURCE=.\src\x_client.c

"$(INTDIR)\x_client.obj"	"$(INTDIR)\x_client.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\x_core.c

"$(INTDIR)\x_core.obj"	"$(INTDIR)\x_core.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\x_data.c

"$(INTDIR)\x_data.obj"	"$(INTDIR)\x_data.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\x_expat.c

!IF  "$(CFG)" == "xrpc - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(NMIT_SHARED_ROOT)/src/native/slib/include" /I "$(NMIT_SHARED_ROOT)/src/native/zlib" /I "include" /I "../ucs2lib/include" /I "$(NMIT_SHARED_ROOT)/src/native/expat/lib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "USE_EXPAT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\xrpc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\x_expat.obj"	"$(INTDIR)\x_expat.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "xrpc - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GX /Z7 /Od /I "$(NMIT_SHARED_ROOT)/src/native/slib/include" /I "$(NMIT_SHARED_ROOT)/src/native/zlib" /I "include" /I "../ucs2lib/include" /I "$(NMIT_SHARED_ROOT)/src/native/expat/lib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "USE_EXPAT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\xrpc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\x_expat.obj"	"$(INTDIR)\x_expat.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\x_libxml.c

"$(INTDIR)\x_libxml.obj"	"$(INTDIR)\x_libxml.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\x_parse.c

"$(INTDIR)\x_parse.obj"	"$(INTDIR)\x_parse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\x_server.c

"$(INTDIR)\x_server.obj"	"$(INTDIR)\x_server.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\x_util.c

"$(INTDIR)\x_util.obj"	"$(INTDIR)\x_util.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

