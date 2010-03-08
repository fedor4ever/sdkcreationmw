# -*- Mode: makefile -*-
#
# $Id: Makefile,v 1.53 2006/03/20 06:50:24 slava Exp $
#
# Makefile for libslava.a
#
# Copyright (C) 2000-2006 by Slava Monich
#
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions 
# are met: 
#
#   1.Redistributions of source code must retain the above copyright 
#     notice, this list of conditions and the following disclaimer. 
#   2.Redistributions in binary form must reproduce the above copyright 
#     notice, this list of conditions and the following disclaimer 
#     in the documentation and/or other materials provided with the 
#     distribution. 
#
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
# IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING 
# IN ANY WAY OUT OF THE USE OR INABILITY TO USE THIS SOFTWARE, EVEN 
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
#
# The views and conclusions contained in the software and documentation 
# are those of the authors and should not be interpreted as representing 
# any official policies, either expressed or implied.
#

.PHONY: clean veryclean nodepend all debug release

CC = gcc
DEBUG_FLAGS = -g
RELEASE_FLAGS = -O2
PROFILE_FLAGS = -pg

INCLUDES = -I./include -I./src -I../expat/lib
DEFINES = -D_REENTRANT -D_HAVE_EXPAT
DEBUG_DEFINES = -DDEBUG=1 -DDEBUG_MEM

WARNINGS = -Wall -Wstrict-prototypes -Wshadow -Wwrite-strings \
 -Waggregate-return -Wnested-externs

CPPFLAGS = $(INCLUDES) $(DEFINES)
CFLAGS = $(WARNINGS)
ARFLAGS = rc

#
# Targets
#

RELEASE_LIB = libslava.a
PROFILE_LIB = libslavap.a
DEBUG_LIB = libslavad.a

RESULTS = $(DEBUG_LIB) $(RELEASE_LIB) $(PROFILE_LIB)

#
# (More or less) platform independent sources
#

SRC = s_base32.c s_base64.c s_bitset.c s_buf.c s_dom.c s_event.c s_file.c \
  s_fio.c s_fmem.c s_fnull.c s_fsock.c s_fsplit.c s_futil.c s_fwrap.c \
  s_fzio.c s_fzip.c s_hash.c s_hist.c s_itr.c s_itra.c s_itrc.c s_itrf.c \
  s_itrs.c s_lib.c s_lock.c s_math.c s_md.c s_md5.c s_mem.c s_mpm.c \
  s_mutex.c s_net.c s_opt.c s_prop.c s_propx.c s_queue.c s_random.c s_sha1.c \
  s_stack.c s_str.c s_strbuf.c s_thread.c s_time.c s_trace.c s_util.c \
  s_vector.c s_wkq.c s_xml.c s_xmlp.c

#
# Platform specific sources
#

SRC1 = u_event.c u_futil.c u_mutex.c u_thread.c u_trace.c

#
# Special target (dependency rules)
#

DEPENDS = Makefile.dep

#
# Directories
#

SRC_DIR = ./src
SRC1_DIR = ./src/unix
BUILD_DIR = build
DEBUG_BUILD_DIR = $(BUILD_DIR)/debug
RELEASE_BUILD_DIR = $(BUILD_DIR)/release
PROFILE_BUILD_DIR = $(RELEASE_BUILD_DIR)/profile

SRC_FILES = \
  $(SRC:%=$(SRC_DIR)/%) \
  $(SRC1:%=$(SRC1_DIR)/%)
DEBUG_OBJS = \
  $(SRC:%.c=$(DEBUG_BUILD_DIR)/%.o) \
  $(SRC1:%.c=$(DEBUG_BUILD_DIR)/%.o)
RELEASE_OBJS = \
  $(SRC:%.c=$(RELEASE_BUILD_DIR)/%.o) \
  $(SRC1:%.c=$(RELEASE_BUILD_DIR)/%.o)
PROFILE_OBJS = \
  $(SRC:%.c=$(PROFILE_BUILD_DIR)/%.o) \
  $(SRC1:%.c=$(PROFILE_BUILD_DIR)/%.o)

#
# Rules
#

all: debug release

debug: debug_build_dir $(DEBUG_LIB) 

debug_build_dir: $(DEBUG_BUILD_DIR)

release: release_build_dir $(RELEASE_LIB) 

release_build_dir: $(RELEASE_BUILD_DIR) 

profile: profile_build_dir $(PROFILE_LIB) 

profile_build_dir: $(PROFILE_BUILD_DIR) 

clean:
	-rm -fr core *~ $(SRC_DIR)/*~ ./include/*~ $(RESULTS) $(BUILD_DIR)

nodepend:
	-rm -fr $(DEPENDS)

veryclean: clean nodepend

$(DEBUG_BUILD_DIR):
	mkdir -p $@

$(RELEASE_BUILD_DIR):
	mkdir -p $@

$(PROFILE_BUILD_DIR):
	mkdir -p $@

$(DEBUG_LIB): $(DEBUG_OBJS)
	$(AR) $(ARFLAGS) $@ $?
	ranlib $@

$(RELEASE_LIB): $(RELEASE_OBJS)
	$(AR) $(ARFLAGS) $@ $?
	ranlib $@

$(PROFILE_LIB): $(PROFILE_OBJS)
	$(AR) $(ARFLAGS) $@ $?
	ranlib $@

#
# Pattern Rules
#

$(DEBUG_BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c $(DEBUG_FLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_DEFINES) $< -o $@

$(DEBUG_BUILD_DIR)/%.o : $(SRC1_DIR)/%.c
	$(CC) -c $(DEBUG_FLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_DEFINES) $< -o $@

$(RELEASE_BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c $(RELEASE_FLAGS) $(CFLAGS) $(CPPFLAGS) $< -o $@

$(RELEASE_BUILD_DIR)/%.o : $(SRC1_DIR)/%.c
	$(CC) -c $(RELEASE_FLAGS) $(CFLAGS) $(CPPFLAGS) $< -o $@

$(PROFILE_BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c $(RELEASE_FLAGS) $(PROFILE_FLAGS) $(CFLAGS) $(CPPFLAGS) $< -o $@

$(PROFILE_BUILD_DIR)/%.o : $(SRC1_DIR)/%.c
	$(CC) -c $(RELEASE_FLAGS) $(PROFILE_FLAGS) $(CFLAGS) $(CPPFLAGS) $< -o $@

#
# Makefile.dep is automatically generated if it's missing 
#
ifdef _FAST_
DEPENDS_DEPENDS = 
else
DEPENDS_DEPENDS = $(SRC_FILES)
endif

$(DEPENDS): $(DEPENDS_DEPENDS)
	@echo "$(MAKE): Updating dependencies"
	@echo "# -*- Mode: makefile -*-" > $@
	@echo "# This file is generated automatically." >> $@
	@echo "# Run 'make veryclean' and  'make' to update it." >> $@
	@$(CC) -MM $(CPPFLAGS) $(DEBUG_DEFINES) $^ | \
	sed "s/^.*:/\\$$\(DEBUG_BUILD_DIR)\\/&/g" >> $@
	@$(CC) -MM $(CPPFLAGS) $^ | \
	sed "s/^.*:/\\$$\(RELEASE_BUILD_DIR)\\/&/g" >> $@
	@$(CC) -MM $(CPPFLAGS) $^ | \
	sed "s/^.*:/\\$$\(PROFILE_BUILD_DIR)\\/&/g" >> $@

include $(DEPENDS)
