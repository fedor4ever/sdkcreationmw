# -*- Mode: makefile -*-
#
#  $Id: Makefile,v 1.2 2005/09/22 01:43:36 slava Exp $
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
INCLUDES = -I$(INC_DIR) $(XRPC_INCLUDES) $(XREG_INCLUDES) 
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

DEBUG_XRPC_CLIENT_APP = xrpc_clientd
RELEASE_XRPC_CLIENT_APP = xrpc_client
DEBUG_XRPC_SERVER_APP = xrpc_serverd
RELEASE_XRPC_SERVER_APP = xrpc_server

RESULTS = \
 $(DEBUG_XRPC_CLIENT_APP) \
 $(RELEASE_XRPC_CLIENT_APP) \
 $(DEBUG_XRPC_SERVER_APP) \
 $(RELEASE_XRPC_SERVER_APP)

#
# Source files
#

SRC_CLIENT = xrpc_client.c
SRC_SERVER = xrpc_server.c
SRC = $(SRC_CLIENT) $(SRC_SERVER)

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

debug: $(DEBUG_BUILD_DIR) $(DEBUG_XRPC_CLIENT_APP) $(DEBUG_XRPC_SERVER_APP)

release: $(RELEASE_BUILD_DIR) $(RELEASE_XRPC_CLIENT_APP) $(RELEASE_XRPC_SERVER_APP)

clean:
	-rm -fr core *~ $(SRC_DIR)/*~ $(RESULTS) $(BUILD_DIR)

#
# Common rules
#

include $(CONFIG_DIR)/Rules.mak

#
# Local rules
#

DEBUG_CLIENT_OBJS = $(SRC_CLIENT:%.c=$(DEBUG_BUILD_DIR)/%.o) 
DEBUG_SERVER_OBJS = $(SRC_SERVER:%.c=$(DEBUG_BUILD_DIR)/%.o) 
RELEASE_CLIENT_OBJS = $(SRC_CLIENT:%.c=$(RELEASE_BUILD_DIR)/%.o)
RELEASE_SERVER_OBJS = $(SRC_SERVER:%.c=$(RELEASE_BUILD_DIR)/%.o)

$(DEBUG_XRPC_CLIENT_APP): $(DEBUG_CLIENT_OBJS)
	$(LD) $(LDFLAGS) $(LD_DEBUG_FLAGS) -o $@ $^ $(LD_DEBUG_LIBS) $(LD_PLATFORM_LIBS)

$(RELEASE_XRPC_CLIENT_APP): $(DEBUG_CLIENT_OBJS)
	$(LD) $(LDFLAGS) $(LD_RELEASE_FLAGS) -o $@ $^ $(LD_RELEASE_LIBS) $(LD_PLATFORM_LIBS)

$(DEBUG_XRPC_SERVER_APP): $(DEBUG_SERVER_OBJS)
	$(LD) $(LDFLAGS) $(LD_DEBUG_FLAGS) -o $@ $^ $(LD_DEBUG_LIBS) $(LD_PLATFORM_LIBS)

$(RELEASE_XRPC_SERVER_APP): $(DEBUG_SERVER_OBJS)
	$(LD) $(LDFLAGS) $(LD_RELEASE_FLAGS) -o $@ $^ $(LD_RELEASE_LIBS) $(LD_PLATFORM_LIBS)

#
# Dependencies
#

include $(DEPENDS)
