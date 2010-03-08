#
# Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
RESOURCEDIR=$(EPOCROOT)epoc32\winscw\c
else
ZDIR=$(EPOCROOT)epoc32\data\z
RESOURCEDIR=$(EPOCROOT)epoc32\data\z
endif


# ----------------------------------------------------------------------------
# TODO: Configure these
# ----------------------------------------------------------------------------

TARGETDIR=$(ZDIR)\resource\apps
BMPTARGETDIR=$(RESOURCEDIR)\resource\apps

ICONTARGETFILENAME=$(TARGETDIR)\localization_aif.mif
#BITMAP_A_FILENAME=$(BMPTARGETDIR)\Localization_01.mbm
#BITMAP_B_FILENAME=$(BMPTARGETDIR)\Localization_03.mbm
BITMAP_A_FILENAME=$(TARGETDIR)\Localization_01.mbm
BITMAP_B_FILENAME=$(TARGETDIR)\Localization_03.mbm
BITMAP_C_FILENAME=$(TARGETDIR)\Localization_31.mbm
BITMAP_D_FILENAME=$(TARGETDIR)\Localization_32.mbm
BITMAP_E_FILENAME=$(TARGETDIR)\Localization_33.mbm

ICONDIR=..\gfx
BITMAPDIR=..\data

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
	mifconv $(ICONTARGETFILENAME) \
		/c32 $(ICONDIR)\qgn_menu_localization.svg
	mifconv $(BITMAP_A_FILENAME) \
		/c24 $(BITMAPDIR)\bitmap_a.bmp
	mifconv $(BITMAP_B_FILENAME) \
		/c24 $(BITMAPDIR)\bitmap_b.bmp
	mifconv $(BITMAP_C_FILENAME) \
		/c24 $(BITMAPDIR)\bitmap_c.bmp
	mifconv $(BITMAP_D_FILENAME) \
		/c24 $(BITMAPDIR)\bitmap_d.bmp
	mifconv $(BITMAP_E_FILENAME) \
		/c24 $(BITMAPDIR)\bitmap_e.bmp

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing

