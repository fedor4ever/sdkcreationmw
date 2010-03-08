#
# Copyright (c) 2000-2006 Nokia Corporation and/or its subsidiary(-ies).
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

!IFNDEF EPOCROOT
EPOCROOT = \
!ENDIF

TargetExe = sbt.exe
Workspace = ..\Bluetooth.dsw
ReleaseExeDir = ..\sbt\build\Release
ReleaseTarget = "sbt - Win32 Release"
WinsCwReleaseDir =  $(EPOCROOT)epoc32\release\winscw\udeb

!INCLUDE ..\..\..\emumenubar\group\winexe.mk
