# Microsoft Developer Studio Generated NMAKE File, Based on wmlsenc.dsp
!IF "$(CFG)" == ""
CFG=wmlsenc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to wmlsenc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "wmlsenc - Win32 Release" && "$(CFG)" != "wmlsenc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "wmlsenc - Win32 Release"

OUTDIR=.\build/release
INTDIR=.\build/release
# Begin Custom Macros
OutDir=.\build/release
# End Custom Macros

ALL : "$(OUTDIR)\wmlsenc.lib"


CLEAN :
	-@erase "$(INTDIR)\bytecode.obj"
	-@erase "$(INTDIR)\codenode.obj"
	-@erase "$(INTDIR)\conspool.obj"
	-@erase "$(INTDIR)\funcpool.obj"
	-@erase "$(INTDIR)\lex.obj"
	-@erase "$(INTDIR)\library.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\memmana.obj"
	-@erase "$(INTDIR)\pragpool.obj"
	-@erase "$(INTDIR)\symtable.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wmlsyacc.obj"
	-@erase "$(OUTDIR)\wmlsenc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "$(RAINBOW_HOME)/rb_wps/server/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "STANDARD_LIBRARIES" /D "CRYPTO_LIBRARIES" /D yyerror=WMLS_yyerror /D alloca=malloc /Fp"$(INTDIR)\wmlsenc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wmlsenc.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\wmlsenc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bytecode.obj" \
	"$(INTDIR)\codenode.obj" \
	"$(INTDIR)\conspool.obj" \
	"$(INTDIR)\funcpool.obj" \
	"$(INTDIR)\lex.obj" \
	"$(INTDIR)\library.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\memmana.obj" \
	"$(INTDIR)\pragpool.obj" \
	"$(INTDIR)\symtable.obj" \
	"$(INTDIR)\wmlsyacc.obj"

"$(OUTDIR)\wmlsenc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"

OUTDIR=.\build/debug
INTDIR=.\build/debug
# Begin Custom Macros
OutDir=.\build/debug
# End Custom Macros

ALL : "$(OUTDIR)\wmlsenc.lib" "$(OUTDIR)\wmlsenc.bsc"


CLEAN :
	-@erase "$(INTDIR)\bytecode.obj"
	-@erase "$(INTDIR)\bytecode.sbr"
	-@erase "$(INTDIR)\codenode.obj"
	-@erase "$(INTDIR)\codenode.sbr"
	-@erase "$(INTDIR)\conspool.obj"
	-@erase "$(INTDIR)\conspool.sbr"
	-@erase "$(INTDIR)\funcpool.obj"
	-@erase "$(INTDIR)\funcpool.sbr"
	-@erase "$(INTDIR)\lex.obj"
	-@erase "$(INTDIR)\lex.sbr"
	-@erase "$(INTDIR)\library.obj"
	-@erase "$(INTDIR)\library.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\memmana.obj"
	-@erase "$(INTDIR)\memmana.sbr"
	-@erase "$(INTDIR)\pragpool.obj"
	-@erase "$(INTDIR)\pragpool.sbr"
	-@erase "$(INTDIR)\symtable.obj"
	-@erase "$(INTDIR)\symtable.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wmlsyacc.obj"
	-@erase "$(INTDIR)\wmlsyacc.sbr"
	-@erase "$(OUTDIR)\wmlsenc.bsc"
	-@erase "$(OUTDIR)\wmlsenc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "$(RAINBOW_HOME)/rb_wps/server/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "STANDARD_LIBRARIES" /D "CRYPTO_LIBRARIES" /D yyerror=WMLS_yyerror /D alloca=malloc /FR"$(INTDIR)\\" /Fp"$(INTDIR)\wmlsenc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wmlsenc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\bytecode.sbr" \
	"$(INTDIR)\codenode.sbr" \
	"$(INTDIR)\conspool.sbr" \
	"$(INTDIR)\funcpool.sbr" \
	"$(INTDIR)\lex.sbr" \
	"$(INTDIR)\library.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\memmana.sbr" \
	"$(INTDIR)\pragpool.sbr" \
	"$(INTDIR)\symtable.sbr" \
	"$(INTDIR)\wmlsyacc.sbr"

"$(OUTDIR)\wmlsenc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\wmlsenc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bytecode.obj" \
	"$(INTDIR)\codenode.obj" \
	"$(INTDIR)\conspool.obj" \
	"$(INTDIR)\funcpool.obj" \
	"$(INTDIR)\lex.obj" \
	"$(INTDIR)\library.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\memmana.obj" \
	"$(INTDIR)\pragpool.obj" \
	"$(INTDIR)\symtable.obj" \
	"$(INTDIR)\wmlsyacc.obj"

"$(OUTDIR)\wmlsenc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("wmlsenc.dep")
!INCLUDE "wmlsenc.dep"
!ELSE 
!MESSAGE Warning: cannot find "wmlsenc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "wmlsenc - Win32 Release" || "$(CFG)" == "wmlsenc - Win32 Debug"
SOURCE=.\src\bytecode.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\bytecode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\bytecode.obj"	"$(INTDIR)\bytecode.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\codenode.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\codenode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\codenode.obj"	"$(INTDIR)\codenode.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\conspool.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\conspool.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\conspool.obj"	"$(INTDIR)\conspool.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\funcpool.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\funcpool.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\funcpool.obj"	"$(INTDIR)\funcpool.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lex.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\lex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\lex.obj"	"$(INTDIR)\lex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\library.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\library.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\library.obj"	"$(INTDIR)\library.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\main.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\memmana.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\memmana.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\memmana.obj"	"$(INTDIR)\memmana.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\pragpool.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\pragpool.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\pragpool.obj"	"$(INTDIR)\pragpool.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\symtable.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\symtable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\symtable.obj"	"$(INTDIR)\symtable.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\wmlsyacc.c

!IF  "$(CFG)" == "wmlsenc - Win32 Release"


"$(INTDIR)\wmlsyacc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wmlsenc - Win32 Debug"


"$(INTDIR)\wmlsyacc.obj"	"$(INTDIR)\wmlsyacc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

