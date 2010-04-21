#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\private\E0000228
ICONTARGETFILENAME=$(TARGETDIR)\Listbox_bitmaps.mif

HEADERDIR=$(EPOCROOT)epoc32\include
HEADERFILENAME=$(HEADERDIR)\Listbox_bitmaps.mbg

ICONDIR=..\gfx

do_nothing :
		@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
		mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/c32,8 $(ICONDIR)\listbox_icon.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
		@echo $(HEADERFILENAME)&& \
		@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
