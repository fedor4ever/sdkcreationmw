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
#  $Id: reglib.mk,v 1.2 2005/07/13 20:45:15 slava Exp $
#
#  Makefile for compiling reglib.lib (Forum Nokia Product Registration library)
#
#  It is not possible to compile this component using standard Symbian 
#  development tools. It's not a Symbian library, it's a Win32 static
#  library.
#
# ============================================================================

TargetName = reglib
ProjectDir = ..\..\Registration\native\$(TargetName)
DebugLibDir = $(ProjectDir)\build\lib
ReleaseLibDir = $(ProjectDir)\build\lib

TargetLib = $(TargetName).lib
DebugTargetLib = $(TargetName)d.lib

Workspace = $(ProjectDir)\reglib.dsw
DebugTarget= "reglib - Win32 Debug"
ReleaseTarget = "reglib - Win32 Release"

!INCLUDE winlib.mk
