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

!INCLUDE config.mk

TargetName = emulator
ProjectDir = ..\emulator
ReleaseExeDir = $(ProjectDir)\build\$(TargetName)\release

TargetExe = $(TargetName).exe

Workspace = ..\MIDP.dsw
ReleaseTarget = "emulator - Win32 Release"
WinsCwReleaseDir = $(MidpBinDir)

!INCLUDE ..\..\Ecmt\group\winexe.mk
