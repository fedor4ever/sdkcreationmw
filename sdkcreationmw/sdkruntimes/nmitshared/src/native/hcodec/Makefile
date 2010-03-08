# -*- Mode: makefile -*-
#
#  $Id: Makefile,v 1.3 2005/09/22 01:34:28 slava Exp $
#
#  Copyright (c) 2000 - 2005 Nokia Corporation.
#  This material, including documentation and any related
#  computer programs, is protected by copyright controlled by
#  Nokia Corporation. All rights are reserved. Copying,
#  including reproducing, storing, adapting or translating, any
#  or all of this material requires the prior written consent of
#  Nokia Corporation. This material also contains confidential
#  information which may not be disclosed to others without the
#  prior written consent of Nokia Corporation.
#

.PHONY: all clean 

#
# Include master config file
#

CONFIG_DIR = ../../build
include $(CONFIG_DIR)/Config.mak

#
# Directories
#

SRC_DIR = ./src
INC_DIR = ./include

#
# Targets
#

RELEASE_LIB = libhcodec.a
DEBUG_LIB = libhcodecd.a
RESULTS = $(DEBUG_LIB) $(RELEASE_LIB) 

#
# Source files
#

SRC = hc_decoder.c hc_encoder.c hc_pages.c hc_util.c other_util.c

#
# Rules
#

all: debug release

clean:
	-rm -fr core *~ $(SRC_DIR)/*~ $(INC_DIR)/*~ $(RESULTS) $(BUILD_DIR)

#
# Common rules
#

include $(CONFIG_DIR)/Rules.mak

#
# Dependencies
#

include $(DEPENDS)
