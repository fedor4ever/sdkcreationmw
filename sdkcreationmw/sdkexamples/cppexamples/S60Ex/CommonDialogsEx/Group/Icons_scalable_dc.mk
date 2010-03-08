#
# Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  icons makefile for project CommonDialogsEx
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\CommonDialogsEx_aif.mif

ICONDIR=..\gfx

MAKMAKE : ;

BLD : ;

CLEAN : ;

LIB : ;

CLEANLIB : ;

RESOURCE :$(ICONTARGETFILENAME)
$(ICONTARGETFILENAME): $(ICONDIR)\qgn_menu_CommonDialogsEx.svg
	mifconv $(ICONTARGETFILENAME) \
		/c32 ..\gfx\qgn_menu_CommonDialogsEx.svg
		
FREEZE : ;

SAVESPACE : ;

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : ;

