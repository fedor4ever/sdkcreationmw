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
#  $Id: EcmtGw.mk,v 1.4 2005/07/13 23:07:37 slava Exp $
#
#  Makefile for compiling EcmtGw.exe
#
# ============================================================================

TargetName = EcmtGw
ProjectDir = ..\gateway
ReleaseExeDir = $(ProjectDir)\ui\build\Release

TargetExe = $(TargetName).exe

Workspace = $(ProjectDir)\$(TargetName).dsw
ReleaseTarget = "EcmtGwUi - Win32 Release"

!INCLUDE winexe.mk
