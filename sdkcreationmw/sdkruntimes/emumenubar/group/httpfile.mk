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
#  $Id: httpfile.mk,v 1.1 2005/12/19 18:59:04 slava Exp $
#
#  Makefile for compiling httpfile.lib
#
#  It is not possible to compile this component using standard Symbian 
#  development tools. It's not a Symbian library, it's a Win32 static
#  library.
#
# ============================================================================

TargetName = httpfile
ProjectDir = ..\..\NmitShared\src\native\$(TargetName)
DebugLibDir = $(ProjectDir)\build
ReleaseLibDir = $(ProjectDir)\build

TargetLib = $(TargetName).lib
DebugTargetLib = $(TargetName)d.lib

Workspace = $(ProjectDir)\httpfile.dsw
DebugTarget= "httpfile - Win32 Debug"
ReleaseTarget = "httpfile - Win32 Release"

!INCLUDE winlib.mk
