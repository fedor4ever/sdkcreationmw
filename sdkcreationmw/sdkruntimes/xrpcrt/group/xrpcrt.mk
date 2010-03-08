#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#
# ============================================================================
#
#  $Id: xrpcrt.mk,v 1.1 2005/04/06 05:36:41 slava Exp $
#
#  Makefile for compiling xrpcrt.dll
#
#  It is not possible to compile this component using standard Symbian 
#  development tools.
#
# ============================================================================

!IF "$(PLATFORM)" != "WINS" && "$(PLATFORM)" != "WINSCW"  && "$(PLATFORM)" != "VC6"
!ERROR "Not a WINS or WINSCW build!"
!ENDIF

OutDir = build
DebugDir = $(OutDir)\debug
ReleaseDir = $(OutDir)\release

TargetName = xrpcrt
TargetExe = $(TargetName).dll
TargetLib = $(TargetName).lib

Workspace = xrpcrt.dsw
DebugTarget= "xrpcrt - Win32 Debug"
ReleaseTarget = "xrpcrt - Win32 Release"

WinsDebugExe = $(EPOCROOT)epoc32\release\wins\udeb\$(TargetExe)
WinsDebugLib = $(EPOCROOT)epoc32\release\wins\udeb\$(TargetLib)
WinsReleaseExe = $(EPOCROOT)epoc32\release\wins\urel\$(TargetExe)
WinsReleaseLib = $(EPOCROOT)epoc32\release\wins\urel\$(TargetLib)
WinsCwDebugExe = $(EPOCROOT)epoc32\release\winscw\udeb\$(TargetExe)
WinsCwDebugLib = $(EPOCROOT)epoc32\release\winscw\udeb\$(TargetLib)
WinsCwReleaseExe = $(EPOCROOT)epoc32\release\winscw\urel\$(TargetExe)
WinsCwReleaseLib = $(EPOCROOT)epoc32\release\winscw\urel\$(TargetLib)

!IFNDEF EpocRoot
EpocRoot = \
!ENDIF

# Targets

RELEASABLES:
!IF "$(PLATFORM)" == "WINS"
!IF "$(CFG)" != "UREL"
    @echo $(WinsDebugExe)
    @echo $(WinsDebugLib)
!ENDIF
!IF "$(CFG)" != "UDEB"
    @echo $(WinsReleaseExe)
    @echo $(WinsReleaseLib)
!ENDIF
!ENDIF
!IF "$(PLATFORM)" == "WINSCW"
!IF "$(CFG)" != "UREL"
    @echo $(WinsCwDebugExe)
    @echo $(WinsCwDebugLib)
!ENDIF
!IF "$(CFG)" != "UDEB"
    @echo $(WinsCwReleaseExe)
    @echo $(WinsCwReleaseLib)
!ENDIF
!ENDIF

CLEAN:
!IF "$(PLATFORM)" == "WINS" && "$(CFG)" == "UDEB"
    if exist $(WinsDebugExe) del /F $(WinsDebugExe)
    if exist $(WinsDebugLib) del /F $(WinsDebugLib)
!ENDIF
!IF "$(PLATFORM)" == "WINS" && "$(CFG)" == "UREL"
    if exist $(WinsReleaseExe) del /F $(WinsReleaseExe)
    if exist $(WinsReleaseLib) del /F $(WinsReleaseLib)
!ENDIF
!IF "$(PLATFORM)" == "WINSCW" && "$(CFG)" == "UDEB"
    if exist $(WinsCwDebugExe) del /F $(WinsCwDebugExe)
    if exist $(WinsCwDebugLib) del /F $(WinsCwDebugLib)
!ENDIF
!IF "$(PLATFORM)" == "WINSCW" && "$(CFG)" == "UREL"
    if exist $(WinsCwReleaseExe) del /F $(WinsCwReleaseExe)
    if exist $(WinsCwReleaseLib) del /F $(WinsCwReleaseLib)
!ENDIF
    msdev $(Workspace) /MAKE $(ReleaseTarget) /USEENV /CLEAN
    if exist $(ReleaseDir) rd /q /s $(ReleaseDir)
!IF "$(CFG)" != "UREL"
    msdev $(Workspace) /MAKE $(DebugTarget) /USEENV /CLEAN
    if exist $(DebugTarget) rd /q /s $(DebugTarget)
!ENDIF

BLD:
!IF "$(CFG)" != "UREL"
    msdev $(Workspace) /MAKE $(DebugTarget) /USEENV
!ENDIF
    msdev $(Workspace) /MAKE $(ReleaseTarget) /USEENV

MAKMAKE FINAL FREEZE LIB CLEANLIB RESOURCE SAVESPACE : 
    @rem donothing
