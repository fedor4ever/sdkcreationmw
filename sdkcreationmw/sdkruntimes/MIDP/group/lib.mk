#
# Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
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

!INCLUDE config.mk

SrcJar = ..\..\..\..\epoc32\tools\java\sei\$(PLATFORM)\udeb\$(MidpProduct)\lib\$(MidpProduct).jar
DestJar = $(MidpLibDir)\$(MidpProduct).jar

DistEpoc32 = $(MidpBinDir)\epoc32
PrefsDir = $(DistEpoc32)\tools\java\sei\winscw\udeb\$(MidpProduct)\symbian\lib

# Command to remove directory tree
RmTree = rd /s /q

# Targets

# Separate debug and release builds don't make sense here.
# We are only building our stuff in release pass to avoid
# building it twice

RELEASABLES:
    @echo $(DestJar)

CLEAN:
    if exist "$(DestJar)" del "$(DestJar)"
    if exist "$(DistEpoc32)" $(RmTree) "$(DistEpoc32)"

BLD:
    if not exist "$(PrefsDir)" md "$(PrefsDir)"
    if not exist "$(MidpLibDir)" md "$(MidpLibDir)"
!IF "$(CFG)" != "UDEB"
# copy "$(SrcJar)" "$(DestJar)" this is commented out as the source dir epoc32\tools\java\sei\WINSCW\udeb\hmidps60v31\lib\hmidps60v31.jar is not available
!ENDIF

MAKMAKE FINAL FREEZE LIB CLEANLIB RESOURCE SAVESPACE :
   @rem donothing
