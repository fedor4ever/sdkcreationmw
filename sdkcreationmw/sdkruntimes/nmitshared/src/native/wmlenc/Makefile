# -*- Mode: makefile -*-
#
#  $Id: Makefile,v 1.5 2005/09/22 01:34:29 slava Exp $
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

#
# Compiler settings
# 
# NOTE: with -O2 and -O3 gcc produces code that does not work (POOL_ALLOC
# macro). -O1 works fine
#

RELEASE_FLAGS = -O1
INCLUDES = -I$(SRC_DIR) -I$(RB_HOME)/rb_wps/server/include

#
# Targets
#

RELEASE_LIB = libwmlenc.a
DEBUG_LIB = libwmlencd.a
RESULTS = $(DEBUG_LIB) $(RELEASE_LIB) 

#
# Source files
#

SRC = chenc.c chmap.c chtables.c codeload.c mibenum.c wmlarray.c wmlbin.c \
  wmlhndl.c wmllog.c wmlmain.c wmlsegm.c wmlstruc.c wmlvalid.c wmlvar.c \
  xmlbuf.c xmlcomm.c xmlconst.c xmlcp.c xmlctyp.c xmllex.c xmlmem.c \
  xmlmsg.c xmlpars.c xmlpool.c xmlrules.c xmlstr.c xmlstruc.c xmlsymt.c \
  xmlvc.c

#
# Rules
#

all: debug release

clean:
	-rm -fr core core.* *~ wmlenc wmlencd $(SRC_DIR)/*~ $(RESULTS) $(BUILD_DIR)

#
# Common rules
#

include $(CONFIG_DIR)/Rules.mak

#
# Additional rules
#

wmlencd: debug
	$(CC) -o $@ -I./include -I$(SLIB_DIR)/include \
$(DEBUG_FLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_DEFINES) \
wmlenc.c \
$(DEBUG_LIB) \
$(RB_WPS)/bld/unix/libwpsd.a \
$(SLIB_DIR)/libslavad.a \
-lpthread -lm $(LD_PLATFORM_LIBS)

wmlenc: release
	$(CC) -o $@ -I./include -I$(SLIB_DIR)/include \
$(RELEASE_FLAGS) $(CFLAGS) $(CPPFLAGS) $(RELEASE_DEFINES) \
wmlenc.c \
$(RELEASE_LIB) \
$(RB_WPS)/bld/unix/libwps.a \
$(SLIB_DIR)/libslava.a \
-lpthread -lm $(LD_PLATFORM_LIBS)

#
# Dependencies
#

include $(DEPENDS)
