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
#  $Id: sdkw.mk,v 1.1 2005/11/05 15:25:06 slava Exp $
#
#  Makefile for compiling sdkw.exe
#
# ============================================================================

!IFNDEF EPOCROOT
EPOCROOT = \
!ENDIF

ProjectDir = ..
TargetExe = sdkw.exe
ReleaseExeDir = $(ProjectDir)\dscli\build\release\windows
WinsCwReleaseDir =  $(EPOCROOT)epoc32\release\winscw\udeb

Workspace = $(ProjectDir)\s60nmit.dsw
ReleaseTarget = "dscli - Win32 Release"

!INCLUDE ..\..\emumenubar\group\winexe.mk
