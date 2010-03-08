#
# Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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

TargetExe = prefs.exe
ProjectDir = ..\..\..\sdkconnectivityfw\emuconnectmanager\epdt_java\launcher
ReleaseExeDir = $(ProjectDir)\build\prefs\release
WinsCwReleaseDir = ..\..\midp\dist\S60_3rd_MIDP_SDK\bin

Workspace = $(ProjectDir)\EcmtMan.dsw
ReleaseTarget = "prefs - Win32 Release"

!INCLUDE winexe.mk
