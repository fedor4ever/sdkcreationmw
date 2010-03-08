# Microsoft Developer Studio Project File - Name="usbdrv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=usbdrv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "usbdrv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "usbdrv.mak" CFG="usbdrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "usbdrv - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "usbdrv - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "usbdrv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build/vc6/release"
# PROP Intermediate_Dir "build/vc6/release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SYS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /I "$(DDK_ROOT)/inc/crt" /I "$(DDK_ROOT)/inc/ddk/wxp" /I "$(DDK_ROOT)/inc/ddk/wdm/wxp" /I "$(DDK_ROOT)/inc/wxp" /I "../../../NmitShared/src/native/slib/include" /D "NDEBUG" /D "_NT_KERNEL" /D _X86_=1 /D i386=1 /D CONDITION_HANDLING=1 /D "STD_CALL" /D "_LIB" /D "WIN32" /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0501 /D WINVER=0x0501 /D FPO=0 /D DEVL=1 /D "WMI_SUPPORT" /D "USB2" /D _USE_NTDDK_H_=1 /FR /YX /FD /QIfdiv- /QIf /Zel /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ntoskrnl.lib hal.lib wmilib.lib hidclass.lib usbd.lib /nologo /base:"0x00010000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry@8" /machine:I386 /out:"build/vc6/release/s60bt.sys" /libpath:"$(DDK_ROOT)\lib\wxp\i386" /section:INIT,d /osversion:5.00 /merge:.rdata=.text /optidata /align:0x80 /driver /subsystem:native,5.00 /opt:ref /merge:_PAGE=PAGE /merge:_TEXT=.text /ignore:4010,4037,4039,4065,4070,4078,4087,4089,4198,4221
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "usbdrv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build/vc6/debug"
# PROP Intermediate_Dir "build/vc6/debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SYS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MDd /W3 /GX /Z7 /Od /Gy /X /I "$(DDK_ROOT)/inc/crt" /I "$(DDK_ROOT)/inc/ddk/wxp" /I "$(DDK_ROOT)/inc/ddk/wdm/wxp" /I "$(DDK_ROOT)/inc/wxp" /I "../../../NmitShared/src/native/slib/include" /D "_DEBUG" /D DEBUG=1 /D "_NT_KERNEL" /D _X86_=1 /D i386=1 /D CONDITION_HANDLING=1 /D "STD_CALL" /D "_LIB" /D "WIN32" /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0501 /D WINVER=0x0501 /D FPO=0 /D DEVL=1 /D "WMI_SUPPORT" /D "USB2" /D _USE_NTDDK_H_=1 /FR /YX /FD /QIfdiv- /QIf /Zel /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ntoskrnl.lib hal.lib wmilib.lib hidclass.lib usbd.lib /nologo /base:"0x00010000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry@8" /incremental:no /debug /debugtype:both /machine:I386 /nodefaultlib /out:"build/vc6/debug/s60bt.sys" /pdbtype:sept /libpath:"$(DDK_ROOT)\lib\wxp\i386" /section:INIT,d /debug:notmapped,full /osversion:5.00 /merge:.rdata=.text /opt:ref /align:0x80 /driver /subsystem:native,5.00 /opt:ref /merge:_PAGE=PAGE /merge:_TEXT=.text /ignore:4010,4037,4039,4065,4070,4078,4087,4089,4198,4221
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "usbdrv - Win32 Release"
# Name "usbdrv - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bt_com.c
# End Source File
# Begin Source File

SOURCE=.\bt_dev.c
# End Source File
# Begin Source File

SOURCE=.\bt_h4.c
# End Source File
# Begin Source File

SOURCE=.\bt_hci.c
# End Source File
# Begin Source File

SOURCE=.\bt_ioctl.c
# End Source File
# Begin Source File

SOURCE=.\bt_pnp.c
# End Source File
# Begin Source File

SOURCE=.\bt_pwr.c
# End Source File
# Begin Source File

SOURCE=.\bt_usb.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bt_ioctl.h
# End Source File
# Begin Source File

SOURCE=.\bt_usb.h
# End Source File
# Begin Source File

SOURCE=.\bt_ver.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\bt_usb.rc
# End Source File
# Begin Source File

SOURCE=.\s60bt.inf
# End Source File
# End Target
# End Project
