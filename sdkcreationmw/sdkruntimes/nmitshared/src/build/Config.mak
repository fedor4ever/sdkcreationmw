# -*- Mode: makefile -*-
#
# ========================================================================
#  Name        : Config.mak
#  Part of     : 
#  Description : 
#  Version     : $Revision: 1.7 $
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
# ========================================================================

#
# This file contains global configration for all native components of 
# Nokia Mobile Browser in toolkit source tree 
#
# Environment: 
#
# GNU make on Unix. Not guaranteed to work with non-GNU make utilities
# or on non-Unix platforms 
#

#
# CONFIG_DIR variable must point to the directory containing this file
#

ifndef CONFIG_DIR
${error CONFIG_DIR must point to the directory containing Config.mak}
endif # CONFIG_DIR

SHARED_SRC = ${shell (cd $(CONFIG_DIR)/..; pwd)}

ifdef RB_HOME
RB_WAE = $(RB_HOME)/rb_wae
RB_WPS = $(RB_HOME)/rb_wps
RB_TEMPEST = $(RB_HOME)/rb_tempest
endif # RB_HOME

#
# Determine the platform
#
OS = ${shell uname -s}
CPU = ${shell uname -m}
ifndef PLATFORM
PLATFORM = ${shell case "$(OS)" in SunOS|Solaris) echo solaris;; Linux) echo linux;; FreeBSD) echo freebsd;; Darwin) echo mac;; *) echo unknown;; esac}
  ifeq "$(PLATFORM)" "unknown"
    ${error Could not determine platform}
  endif
endif #PLATFORM

#
# Compiler settings
#

CC = gcc
DEBUG_FLAGS = -g
RELEASE_FLAGS = -O3
DEBUG_DEFINES = -DDEBUG=1 -D_DEBUG=1
RELEASE_DEFINES = -DNDEBUG=1
WARNINGS = -Wall -Wwrite-strings -Waggregate-return -Wnested-externs
BASE_DEFINES = -D_UNIX -D_REENTRANT
DEFINES = $(BASE_DEFINES)
INCLUDES = -I$(INC_DIR) -I$(SRC_DIR)

ifeq ($(PLATFORM),mac)
BASE_FLAGS = -fno-common $(WARNINGS)
else
BASE_FLAGS = $(WARNINGS)
endif

CFLAGS = $(BASE_FLAGS)
ARFLAGS = rc

#
# Linker settings
#

LD = gcc
LD_DEBUG_FLAGS = -g
LD_RELEASE_FLAGS = 

#
# Directories
#

NATIVE_DIR = $(SHARED_SRC)/native

XRPC_DIR = $(NATIVE_DIR)/xrpc
XREG_DIR = $(XRPC_DIR)/xreg
UCS2_DIR = $(NATIVE_DIR)/ucs2lib
HCODEC_DIR = $(NATIVE_DIR)/hcodec
WMLENC_DIR = $(NATIVE_DIR)/wmlenc
WMLSENC_DIR = $(NATIVE_DIR)/wmlsenc
JNILIB_DIR = $(NATIVE_DIR)/jnilib
3DPARTY_DIR = $(NATIVE_DIR)

# These should be moved over the 3-rd party sources

SLIB_DIR = $(3DPARTY_DIR)/slib
EXPAT_DIR = $(3DPARTY_DIR)/expat

BUILD_DIR = build
DEBUG_BUILD_DIR = $(BUILD_DIR)/debug
RELEASE_BUILD_DIR = $(BUILD_DIR)/release

#
# Include directories
#

ifdef JAVA_HOME
ifeq "$(PLATFORM)" "mac"
JAVA_INCLUDES = -I$(JAVA_HOME)/Headers
else
JAVA_INCLUDES = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(PLATFORM)
endif # mac
endif # JAVA_HOME

NETSIM_INCLUDES = -I$(NETSIM_DIR)/include
XRPC_INCLUDES = -I$(XRPC_DIR)/include
XREG_INCLUDES = -I$(XREG_DIR)/include
UCS2_INCLUDES = -I$(UCS2_DIR)/include
WMLENC_INCLUDES = -I$(WMLENC_DIR)/include
HCODEC_INCLUDES = -I$(HCODEC_DIR)/include
JNILIB_INCLUDES = -I$(JNILIB_DIR)
EXPAT_INCLUDES = -I$(EXPAT_DIR)/lib
SLIB_INCLUDES = -I$(SLIB_DIR)/include
