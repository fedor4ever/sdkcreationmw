# -*- Mode: makefile -*-
#
# $Id: Makefile,v 1.6 2003/05/02 23:04:55 slava Exp $
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

SRC_DIR = ./lib
INC_DIR = ./lib

#
# Compiler settings
# 

WARNINGS = -Wall -Wnested-externs
DEFINES = $(BASE_DEFINES) -D_REENTRANT -DXML_UNICODE
INCLUDES = -I. -I$(INC_DIR)

#
# Targets
#

RELEASE_LIB = libexpat.a
DEBUG_LIB = libexpatd.a
RESULTS = $(DEBUG_LIB) $(RELEASE_LIB) 

#
# Source files
#

SRC = xmlparse.c xmlrole.c xmltok.c

#
# Rules
#

all: debug release

clean:
	-rm -fr *~ $(SRC_DIR)/*~ $(RESULTS) $(BUILD_DIR)

#
# Common rules
#

include $(CONFIG_DIR)/Rules.mak

#
# Dependencies
#

include $(DEPENDS)
