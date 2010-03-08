# Microsoft Developer Studio Project File - Name="slib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=slib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "slib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "slib.mak" CFG="slib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "slib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "slib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "slib - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./src" /I "./include" /D "NDEBUG" /D "_HAVE_EXPAT" /D "_LIB" /D "WIN32" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"slib.lib"

!ELSEIF  "$(CFG)" == "slib - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "./src" /I "./include" /D "_DEBUG" /D "DEBUG_MEM" /D "_HAVE_EXPAT" /D "_LIB" /D "WIN32" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"slibd.lib"

!ENDIF 

# Begin Target

# Name "slib - Win32 Release"
# Name "slib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\s_base32.c
# End Source File
# Begin Source File

SOURCE=.\src\s_base64.c
# End Source File
# Begin Source File

SOURCE=.\src\s_bitset.c
# End Source File
# Begin Source File

SOURCE=.\src\s_buf.c
# End Source File
# Begin Source File

SOURCE=.\src\s_dom.c
# End Source File
# Begin Source File

SOURCE=.\src\s_event.c
# End Source File
# Begin Source File

SOURCE=.\src\s_file.c
# End Source File
# Begin Source File

SOURCE=.\src\s_fio.c
# End Source File
# Begin Source File

SOURCE=.\src\s_fmem.c
# End Source File
# Begin Source File

SOURCE=.\src\s_fnull.c
# End Source File
# Begin Source File

SOURCE=.\src\s_fsock.c
# End Source File
# Begin Source File

SOURCE=.\src\s_fsplit.c
# End Source File
# Begin Source File

SOURCE=.\src\s_futil.c
# End Source File
# Begin Source File

SOURCE=.\src\s_fwrap.c
# End Source File
# Begin Source File

SOURCE=.\src\s_fzio.c
# ADD CPP /I "../zlib"
# End Source File
# Begin Source File

SOURCE=.\src\s_fzip.c
# ADD CPP /I "../zlib"
# End Source File
# Begin Source File

SOURCE=.\src\s_hash.c
# End Source File
# Begin Source File

SOURCE=.\src\s_hist.c
# End Source File
# Begin Source File

SOURCE=.\src\s_itr.c
# End Source File
# Begin Source File

SOURCE=.\src\s_itra.c
# End Source File
# Begin Source File

SOURCE=.\src\s_itrc.c
# End Source File
# Begin Source File

SOURCE=.\src\s_itrf.c
# End Source File
# Begin Source File

SOURCE=.\src\s_itrs.c
# End Source File
# Begin Source File

SOURCE=.\src\s_lib.c
# End Source File
# Begin Source File

SOURCE=.\src\s_lock.c
# End Source File
# Begin Source File

SOURCE=.\src\s_math.c
# End Source File
# Begin Source File

SOURCE=.\src\s_md.c
# End Source File
# Begin Source File

SOURCE=.\src\s_md5.c
# End Source File
# Begin Source File

SOURCE=.\src\s_mem.c
# End Source File
# Begin Source File

SOURCE=.\src\s_mpm.c
# End Source File
# Begin Source File

SOURCE=.\src\s_mutex.c
# End Source File
# Begin Source File

SOURCE=.\src\s_net.c
# End Source File
# Begin Source File

SOURCE=.\src\s_opt.c
# End Source File
# Begin Source File

SOURCE=.\src\s_prop.c
# End Source File
# Begin Source File

SOURCE=.\src\s_propx.c
# End Source File
# Begin Source File

SOURCE=.\src\s_queue.c
# End Source File
# Begin Source File

SOURCE=.\src\s_random.c
# End Source File
# Begin Source File

SOURCE=.\src\s_sha1.c
# End Source File
# Begin Source File

SOURCE=.\src\s_stack.c
# End Source File
# Begin Source File

SOURCE=.\src\s_str.c
# End Source File
# Begin Source File

SOURCE=.\src\s_strbuf.c
# End Source File
# Begin Source File

SOURCE=.\src\s_thread.c
# End Source File
# Begin Source File

SOURCE=.\src\s_time.c
# End Source File
# Begin Source File

SOURCE=.\src\s_trace.c
# End Source File
# Begin Source File

SOURCE=.\src\s_util.c
# End Source File
# Begin Source File

SOURCE=.\src\s_vector.c
# End Source File
# Begin Source File

SOURCE=.\src\s_wkq.c
# End Source File
# Begin Source File

SOURCE=.\src\s_xml.c
# End Source File
# Begin Source File

SOURCE=.\src\s_xmlp.c
# ADD CPP /I "../expat/lib"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\s_base32.h
# End Source File
# Begin Source File

SOURCE=.\include\s_base64.h
# End Source File
# Begin Source File

SOURCE=.\include\s_bitset.h
# End Source File
# Begin Source File

SOURCE=.\include\s_buf.h
# End Source File
# Begin Source File

SOURCE=.\include\s_def.h
# End Source File
# Begin Source File

SOURCE=.\include\s_dom.h
# End Source File
# Begin Source File

SOURCE=.\include\s_event.h
# End Source File
# Begin Source File

SOURCE=.\include\s_file.h
# End Source File
# Begin Source File

SOURCE=.\include\s_hash.h
# End Source File
# Begin Source File

SOURCE=.\include\s_hist.h
# End Source File
# Begin Source File

SOURCE=.\include\s_itr.h
# End Source File
# Begin Source File

SOURCE=.\include\s_lib.h
# End Source File
# Begin Source File

SOURCE=.\include\s_lock.h
# End Source File
# Begin Source File

SOURCE=.\include\s_math.h
# End Source File
# Begin Source File

SOURCE=.\include\s_md.h
# End Source File
# Begin Source File

SOURCE=.\include\s_mem.h
# End Source File
# Begin Source File

SOURCE=.\include\s_mutex.h
# End Source File
# Begin Source File

SOURCE=.\include\s_ntk.h
# End Source File
# Begin Source File

SOURCE=.\include\s_opt.h
# End Source File
# Begin Source File

SOURCE=.\include\s_os.h
# End Source File
# Begin Source File

SOURCE=.\include\s_prop.h
# End Source File
# Begin Source File

SOURCE=.\include\s_queue.h
# End Source File
# Begin Source File

SOURCE=.\include\s_random.h
# End Source File
# Begin Source File

SOURCE=.\include\s_stack.h
# End Source File
# Begin Source File

SOURCE=.\include\s_strbuf.h
# End Source File
# Begin Source File

SOURCE=.\include\s_thread.h
# End Source File
# Begin Source File

SOURCE=.\include\s_unix.h
# End Source File
# Begin Source File

SOURCE=.\include\s_util.h
# End Source File
# Begin Source File

SOURCE=.\include\s_vector.h
# End Source File
# Begin Source File

SOURCE=.\include\s_ver.h
# End Source File
# Begin Source File

SOURCE=.\include\s_win32.h
# End Source File
# Begin Source File

SOURCE=.\include\s_wkq.h
# End Source File
# Begin Source File

SOURCE=.\include\s_xml.h
# End Source File
# End Group
# Begin Group "Private Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\s_fio.h
# End Source File
# Begin Source File

SOURCE=.\src\s_iop.h
# End Source File
# Begin Source File

SOURCE=.\src\s_itrp.h
# End Source File
# Begin Source File

SOURCE=.\src\s_libp.h
# End Source File
# Begin Source File

SOURCE=.\src\s_mdp.h
# End Source File
# Begin Source File

SOURCE=.\src\s_thrp.h
# End Source File
# Begin Source File

SOURCE=.\src\s_xmlp.h
# End Source File
# End Group
# Begin Group "Win32 Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\win32\w_event.c
# End Source File
# Begin Source File

SOURCE=.\src\win32\w_futil.c
# End Source File
# Begin Source File

SOURCE=.\src\win32\w_mutex.c
# End Source File
# Begin Source File

SOURCE=.\src\win32\w_thread.c
# End Source File
# Begin Source File

SOURCE=.\src\win32\w_trace.c
# End Source File
# End Group
# End Target
# End Project
