# -*- Mode: makefile -*-
#
#  $Id: Makefile,v 1.1 2005/10/28 18:00:55 slava Exp $
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

CONFIG_DIR = ../../../build
include $(CONFIG_DIR)/Config.mak

#
# Directories
#

SRC_DIR = .
INC_DIR = .

#
# Compiler settings
#
INCLUDES = -I$(INC_DIR) $(XRPC_INCLUDES) $(XREG_INCLUDES) $(SLIB_INCLUDES)
LD_DEBUG_LIBS = \
 $(XREG_DIR)/libxregd.a \
 $(XRPC_DIR)/libxrpcd.a \
 $(UCS2_DIR)/libucs2d.a \
 $(EXPAT_DIR)/libexpatd.a \
 $(SLIB_DIR)/libslavad.a
LD_RELEASE_LIBS = \
 $(XREG_DIR)/libxreg.a \
 $(XRPC_DIR)/libxrpc.a \
 $(UCS2_DIR)/libucs2.a \
 $(EXPAT_DIR)/libexpat.a \
 $(SLIB_DIR)/libslava.a

LD_PLATFORM_LIBS = -lpthread -lm

#
# Targets
#

DEBUG_XLS_APP = xlsd
RELEASE_XLS_APP = xls
DEBUG_XREGD_APP = xregdd
RELEASE_XREGD_APP = xregd

RESULTS = \
 $(DEBUG_XLS_APP) \
 $(RELEASE_XLS_APP) \
 $(DEBUG_XREGD_APP) \
 $(RELEASE_XREGD_APP)

#
# Source files
#

SRC_XLS = xls.c
SRC_XREGD = xregd.c
SRC = $(SRC_XLS) $(SRC_XREGD)

#
# Rules
#

all: debug release

client: \
 $(DEBUG_BUILD_DIR) \
 $(RELEASE_BUILD_DIR) \
 $(DEBUG_XRPC_CLIENT_APP) \
 $(RELEASE_XRPC_CLIENT_APP) 

server: \
 $(DEBUG_BUILD_DIR) \
 $(RELEASE_BUILD_DIR) \
 $(DEBUG_XRPC_SERVER_APP) \
 $(RELEASE_XRPC_SERVER_APP) 

debug: $(DEBUG_BUILD_DIR) $(DEBUG_XLS_APP) $(DEBUG_XREGD_APP)

release: $(RELEASE_BUILD_DIR) $(RELEASE_XLS_APP) $(RELEASE_XREGD_APP)

clean:
	-rm -fr core *~ $(SRC_DIR)/*~ $(RESULTS) $(BUILD_DIR)

#
# Common rules
#

include $(CONFIG_DIR)/Rules.mak

#
# Local rules
#

DEBUG_XLS_OBJS = $(SRC_XLS:%.c=$(DEBUG_BUILD_DIR)/%.o) 
DEBUG_XREGD_OBJS = $(SRC_XREGD:%.c=$(DEBUG_BUILD_DIR)/%.o) 
RELEASE_XLS_OBJS = $(SRC_XLS:%.c=$(RELEASE_BUILD_DIR)/%.o)
RELEASE_XREGD_OBJS = $(SRC_XREGD:%.c=$(RELEASE_BUILD_DIR)/%.o)

$(DEBUG_XLS_APP): $(DEBUG_XLS_OBJS)
	$(LD) $(LDFLAGS) $(LD_DEBUG_FLAGS) -o $@ $^ $(LD_DEBUG_LIBS) $(LD_PLATFORM_LIBS)

$(RELEASE_XLS_APP): $(DEBUG_XLS_OBJS)
	$(LD) $(LDFLAGS) $(LD_RELEASE_FLAGS) -o $@ $^ $(LD_RELEASE_LIBS) $(LD_PLATFORM_LIBS)

$(DEBUG_XREGD_APP): $(DEBUG_XREGD_OBJS)
	$(LD) $(LDFLAGS) $(LD_DEBUG_FLAGS) -o $@ $^ $(LD_DEBUG_LIBS) $(LD_PLATFORM_LIBS)

$(RELEASE_XREGD_APP): $(DEBUG_XREGD_OBJS)
	$(LD) $(LDFLAGS) $(LD_RELEASE_FLAGS) -o $@ $^ $(LD_RELEASE_LIBS) $(LD_PLATFORM_LIBS)

#
# Dependencies
#

include $(DEPENDS)
