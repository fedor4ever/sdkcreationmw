# -*- Mode: makefile -*-
#
#  $Id: Makefile,v 1.9 2005/10/28 18:00:41 slava Exp $
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
# Compiler settings
#
DEFINES = $(BASE_DEFINES) -DUSE_EXPAT=1
INCLUDES = -I$(INC_DIR) -I$(SRC_DIR) $(EXPAT_INCLUDES) $(SLIB_INCLUDES) \
 $(UCS2_INCLUDES)

#
# Targets
#

RELEASE_LIB = libxrpc.a
DEBUG_LIB = libxrpcd.a
RESULTS = $(DEBUG_LIB) $(RELEASE_LIB) 

#
# Source files
#

SRC = x_client.c x_core.c x_data.c x_expat.c x_parse.c x_server.c x_util.c \
  x_id.c

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
