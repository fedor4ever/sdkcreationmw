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
# Description:  This is file for creating .mif file (scalable icon)
#
# ============================================================================
#  Name	 : Icons_scalable_dc.mk
#  Part of  : HsWidget
#
#  Description: This is file for creating .mif file (scalable icon)
# 
# ============================================================================


CDIR=$(EPOCROOT)epoc32\winscw\c

TARGETDIR=$(CDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\HsWidget.mif
HEADERDIR=$(EPOCROOT)epoc32\include
HEADERFILENAME=$(HEADERDIR)\HsWidget.mbg

ICONDIR=..\gfx

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# NOTE: if you have JUSTINTIME enabled for your S60 3rd FP1 or newer SDK
# and this command crashes, consider adding "/X" to the command line.
# See <http://forum.nokia.com/document/Forum_Nokia_Technical_Library_v1_35/contents/FNTL/Build_process_fails_at_mif_file_creation_in_S60_3rd_Ed_FP1_SDK.htm>
# ----------------------------------------------------------------------------

RESOURCE : $(ICONTARGETFILENAME)

$(ICONTARGETFILENAME) : 
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/c32 $(ICONDIR)\qgn_menu_HsWidget.svg \
		/c32,8 $(ICONDIR)\Sunny-intervals.svg \
		/c32,8 $(ICONDIR)\Sunny.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing

