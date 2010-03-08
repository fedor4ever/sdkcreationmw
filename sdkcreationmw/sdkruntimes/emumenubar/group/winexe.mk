#
# Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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

!IF "$(PLATFORM)" != "WINS" && "$(PLATFORM)" != "WINSCW"  && "$(PLATFORM)" != "VC6"
!ERROR "Not a WINS or WINSCW build!"
!ENDIF

!IFNDEF EPOCROOT
EPOCROOT = \
!ENDIF

!IFNDEF WinsCwReleaseDir
WinsCwReleaseDir = $(EPOCROOT)epoc32\tools\ecmt
!ENDIF
!IFNDEF WinsReleaseDir
WinsReleaseDir = $(WinsCwReleaseDir)
!ENDIF

WinsReleaseExe = $(WinsReleaseDir)\$(TargetExe)
WinsCwReleaseExe = $(WinsCwReleaseDir)\$(TargetExe)

# Targets

RELEASABLES:
!IF "$(PLATFORM)" == "WINS"
!IF "$(CFG)" != "UDEB"
    @echo $(WinsReleaseExe)
!ENDIF
!ENDIF
!IF "$(PLATFORM)" == "WINSCW"
!IF "$(CFG)" != "UDEB"
    @echo $(WinsCwReleaseExe)
!ENDIF
!ENDIF

CLEAN:
!IF "$(PLATFORM)" == "WINS" && "$(CFG)" == "UREL"
    if exist $(WinsReleaseExe) del /F $(WinsReleaseExe)
!ENDIF
!IF "$(PLATFORM)" == "WINSCW" && "$(CFG)" == "UREL"
    if exist $(WinsCwReleaseExe) del /F $(WinsCwReleaseExe)
!ENDIF
    msdev $(Workspace) /MAKE $(ReleaseTarget) /USEENV /CLEAN

BLD:
    msdev $(Workspace) /MAKE $(ReleaseTarget) /USEENV
!IF "$(PLATFORM)" == "WINS" 
!IF "$(CFG)" != "UDEB"
    echo  $(WinsReleaseExe)
    if not exist $(WinsReleaseDir) md $(WinsReleaseDir)
    copy  $(ReleaseExeDir)\$(TargetExe) $(WinsReleaseExe)
!ENDIF
!ENDIF
!IF "$(PLATFORM)" == "WINSCW"
!IF "$(CFG)" != "UDEB"
    echo $(WinsCwReleaseExe)
    if not exist $(WinsCwReleaseDir) md $(WinsCwReleaseDir)
    copy  $(ReleaseExeDir)\$(TargetExe) $(WinsCwReleaseExe)
!ENDIF
!ENDIF

MAKMAKE FINAL FREEZE LIB CLEANLIB RESOURCE SAVESPACE : 
    @rem donothing
