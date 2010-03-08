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

!IF "$(PLATFORM)" != "WINS" && "$(PLATFORM)" != "WINSCW"
!ERROR "Only building Java for WINS or WINSCW platform!"
!ENDIF

!IFNDEF EPOCROOT
EPOCROOT = \
!ENDIF

!INCLUDE config.mk

Ant = ant
BuildFile = ../nei/build.xml
BuildDir = ..\nei\build
JarFile = nei.jar
FinalJarDir = $(MidpBinDir)\epoc32\tools\ecmt
FinalJarFile = $(FinalJarDir)\$(JarFile)

# Targets

# Separate debug and release builds don't make sense here.
# We are only building our stuff in release pass to avoid
# building it twice

RELEASABLES:
    @echo $(FinalJarFile)

CLEAN:
    if exist $(FinalJarFile) del $(FinalJarFile)
!IF "$(CFG)" != "UDEB"
    $(Ant) -f $(BuildFile) clean
!ENDIF


BLD:
# Don't need to create $(MidpBinDir) - it's a subdirectory of $(FinalJarDir)
    if not exist $(FinalJarDir) md $(FinalJarDir)
!IF "$(CFG)" != "UDEB"
    $(Ant) -f $(BuildFile) build
    if exist $(BuildDir)\$(JarFile) copy $(BuildDir)\$(JarFile) $(FinalJarFile)
!ENDIF

MAKMAKE FINAL FREEZE LIB CLEANLIB RESOURCE SAVESPACE :
   @rem donothing
