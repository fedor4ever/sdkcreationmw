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
#  $Id: winlib.mk,v 1.1 2005/07/13 20:45:15 slava Exp $
#
#  Common part of all Win32 makefiles
#
#  Requires the following variables:
#
#    TargetLib
#    DebugTargetLib
#    ReleaseLibDir
#    DebugLibDir
#    ReleaseTarget
#    DebugTarget
#    Workspace
#
# ============================================================================

!IF "$(PLATFORM)" != "WINS" && "$(PLATFORM)" != "WINSCW"  && "$(PLATFORM)" != "VC6"
!ERROR "Not a WINS or WINSCW build!"
!ENDIF

!IFNDEF EPOCROOT
EPOCROOT = \
!ENDIF

WinsDebugDir = $(EPOCROOT)epoc32\release\wins\udeb
WinsReleaseDir = $(EPOCROOT)epoc32\release\wins\urel
WinsCwDebugDir = $(EPOCROOT)epoc32\release\winscw\udeb
WinsCwReleaseDir = $(EPOCROOT)epoc32\release\winscw\urel
WinsDebugLib = $(WinsDebugDir)\$(TargetLib)
WinsReleaseLib = $(WinsReleaseDir)\$(TargetLib)
WinsCwDebugLib = $(WinsCwDebugDir)\$(TargetLib)
WinsCwReleaseLib = $(WinsCwReleaseDir)\$(TargetLib)
WinsDebugLib2 = $(WinsDebugDir)\$(DebugTargetLib)
WinsReleaseLib2 = $(WinsReleaseDir)\$(DebugTargetLib)
WinsCwDebugLib2 = $(WinsCwDebugDir)\$(DebugTargetLib)
WinsCwReleaseLib2 = $(WinsCwReleaseDir)\$(DebugTargetLib)

# Targets

RELEASABLES:
!IF "$(PLATFORM)" == "WINS"
!IF "$(CFG)" != "UREL"
    @echo $(WinsDebugLib)
    @echo $(WinsDebugLib2)
!ENDIF
!IF "$(CFG)" != "UDEB"
    @echo $(WinsReleaseLib)
    @echo $(WinsReleaseLib2)
!ENDIF
!ENDIF
!IF "$(PLATFORM)" == "WINSCW"
!IF "$(CFG)" != "UREL"
    @echo $(WinsCwDebugLib)
    @echo $(WinsCwDebugLib2)
!ENDIF
!IF "$(CFG)" != "UDEB"
    @echo $(WinsCwReleaseLib)
    @echo $(WinsCwReleaseLib2)
!ENDIF
!ENDIF

CLEAN:
!IF "$(PLATFORM)" == "WINS" && "$(CFG)" == "UDEB"
    if exist $(WinsDebugLib) del /F $(WinsDebugLib)
    if exist $(WinsDebugLib2) del /F $(WinsDebugLib2)
!ENDIF
!IF "$(PLATFORM)" == "WINS" && "$(CFG)" == "UREL"
    if exist $(WinsReleaseLib) del /F $(WinsReleaseLib)
    if exist $(WinsReleaseLib2) del /F $(WinsReleaseLib2)
!ENDIF
!IF "$(PLATFORM)" == "WINSCW" && "$(CFG)" == "UDEB"
    if exist $(WinsCwDebugLib) del /F $(WinsCwDebugLib)
    if exist $(WinsCwDebugLib2) del /F $(WinsCwDebugLib2)
!ENDIF
!IF "$(PLATFORM)" == "WINSCW" && "$(CFG)" == "UREL"
    if exist $(WinsCwReleaseLib) del /F $(WinsCwReleaseLib)
    if exist $(WinsCwReleaseLib2) del /F $(WinsCwReleaseLib2)
!ENDIF
    msdev $(Workspace) /MAKE $(ReleaseTarget) /USEENV /CLEAN
!IF "$(CFG)" != "UREL"
    msdev $(Workspace) /MAKE $(DebugTarget) /USEENV /CLEAN
!ENDIF

BLD:
    msdev $(Workspace) /MAKE $(DebugTarget) /USEENV
    msdev $(Workspace) /MAKE $(ReleaseTarget) /USEENV
!IF "$(PLATFORM)" == "WINS" 
!IF "$(CFG)" != "UREL" && EXIST ("$(WinsDebugDir)")
    echo $(WinsDebugLib)
    copy  $(ReleaseLibDir)\$(TargetLib) $(WinsDebugLib)
    echo $(WinsDebugLib2)
    copy  $(DebugLibDir)\$(DebugTargetLib) $(WinsDebugLib2)
!ENDIF
!IF "$(CFG)" != "UDEB" && EXIST ("$(WinsReleaseDir)")
    echo  $(WinsReleaseLib)
    copy  $(ReleaseLibDir)\$(TargetLib) $(WinsReleaseLib)
    echo  $(WinsReleaseLib2)
    copy  $(DebugLibDir)\$(DebugTargetLib) $(WinsReleaseLib2)
!ENDIF
!ENDIF
!IF "$(PLATFORM)" == "WINSCW"
!IF "$(CFG)" != "UREL" && EXIST ("$(WinsCwReleaseDir)")
    echo  $(WinsCwDebugLib)
    copy  $(ReleaseLibDir)\$(TargetLib) $(WinsCwDebugLib)
    echo  $(WinsCwDebugLib2)
    copy  $(DebugLibDir)\$(DebugTargetLib) $(WinsCwDebugLib2)
!ENDIF
!IF "$(CFG)" != "UDEB" && EXIST ("$(WinsCwReleaseDir)")
    echo $(WinsCwReleaseLib)
    copy  $(ReleaseLibDir)\$(TargetLib) $(WinsCwReleaseLib)
    echo $(WinsCwReleaseLib2)
    copy  $(DebugLibDir)\$(DebugTargetLib) $(WinsCwReleaseLib2)
!ENDIF
!ENDIF

MAKMAKE FINAL FREEZE LIB CLEANLIB RESOURCE SAVESPACE : 
    @rem donothing
