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
#  $Id: EcmtMan.mk,v 1.2 2006/01/06 05:08:44 slava Exp $
#
#  Makefile for compiling EcmtMan.exe
#
# ============================================================================

TargetName = EcmtMan
ProjectDir = ..\..\..\sdkconnectivityfw\emuconnectmanager\epdt_java\launcher
ReleaseExeDir = $(ProjectDir)\build\EcmtMan\release

TargetExe = $(TargetName).exe

Workspace = $(ProjectDir)\$(TargetName).dsw
ReleaseTarget = "EcmtMan - Win32 Release"

!INCLUDE winexe.mk
