# Microsoft Developer Studio Generated NMAKE File, Based on expat.dsp
!IF "$(CFG)" == ""
CFG=expat - Win32 Debug
!MESSAGE No configuration specified. Defaulting to expat - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "expat - Win32 Release" && "$(CFG)" != "expat - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "expat.mak" CFG="expat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "expat - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "expat - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "expat - Win32 Release"

OUTDIR=.\build/win32/i386/release
INTDIR=.\build/win32/i386/release
# Begin Custom Macros
OutDir=.\build/win32/i386/release
# End Custom Macros

ALL : "$(OUTDIR)\expat.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\xmlparse.obj"
	-@erase "$(INTDIR)\xmlrole.obj"
	-@erase "$(INTDIR)\xmltok.obj"
	-@erase "$(OUTDIR)\expat.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "." /I "lib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "XML_UNICODE" /D "XML_DTD" /D "XML_NS" /D "_STATIC" /Fp"$(INTDIR)\expat.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"build/win32/i386/expat.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\expat.lib" 
LIB32_OBJS= \
	"$(INTDIR)\xmlparse.obj" \
	"$(INTDIR)\xmlrole.obj" \
	"$(INTDIR)\xmltok.obj"

"$(OUTDIR)\expat.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "expat - Win32 Debug"

OUTDIR=.\build/win32/i386/debug
INTDIR=.\build/win32/i386/debug
# Begin Custom Macros
OutDir=.\build/win32/i386/debug
# End Custom Macros

ALL : "$(OUTDIR)\expatd.lib" ".\build\win32\i386\expat.bsc"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\xmlparse.obj"
	-@erase "$(INTDIR)\xmlparse.sbr"
	-@erase "$(INTDIR)\xmlrole.obj"
	-@erase "$(INTDIR)\xmlrole.sbr"
	-@erase "$(INTDIR)\xmltok.obj"
	-@erase "$(INTDIR)\xmltok.sbr"
	-@erase "$(OUTDIR)\expatd.lib"
	-@erase ".\build\win32\i386\expat.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "." /I "lib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "XML_UNICODE" /D "XML_DTD" /D "XML_NS" /D "_STATIC" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\expat.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"build/win32/i386/expat.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\xmlparse.sbr" \
	"$(INTDIR)\xmlrole.sbr" \
	"$(INTDIR)\xmltok.sbr"

".\build\win32\i386\expat.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\expatd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\xmlparse.obj" \
	"$(INTDIR)\xmlrole.obj" \
	"$(INTDIR)\xmltok.obj"

"$(OUTDIR)\expatd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("expat.dep")
!INCLUDE "expat.dep"
!ELSE 
!MESSAGE Warning: cannot find "expat.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "expat - Win32 Release" || "$(CFG)" == "expat - Win32 Debug"
SOURCE=.\lib\xmlparse.c

!IF  "$(CFG)" == "expat - Win32 Release"


"$(INTDIR)\xmlparse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "expat - Win32 Debug"


"$(INTDIR)\xmlparse.obj"	"$(INTDIR)\xmlparse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\lib\xmlrole.c

!IF  "$(CFG)" == "expat - Win32 Release"


"$(INTDIR)\xmlrole.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "expat - Win32 Debug"


"$(INTDIR)\xmlrole.obj"	"$(INTDIR)\xmlrole.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\lib\xmltok.c

!IF  "$(CFG)" == "expat - Win32 Release"


"$(INTDIR)\xmltok.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "expat - Win32 Debug"


"$(INTDIR)\xmltok.obj"	"$(INTDIR)\xmltok.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\lib\xmltok_impl.c
SOURCE=.\lib\xmltok_ns.c

!ENDIF 

