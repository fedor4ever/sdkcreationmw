# -*- Mode: makefile -*-
#
# ========================================================================
#  Name        : Rules.mak
#  Part of     : 
#  Description : 
#  Version     : $Revision: 1.4 $
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
# This file contains common rules for building various native components 
# of Nokia Mobile Browser and some reusable components
#
# Environment: 
#
# GNU make on Unix. Not guaranteed to work with non-GNU make utilities
# or on non-Unix platforms 
#

.PHONY: debug release depend nodepend veryclean

#
# Input:
#
# SRCxx       list of source files (no directory names)
# SRCxx_DIR   directory where source files are located
# 

SRC_FILES = \
$(SRC:%=$(SRC_DIR)/%) \
$(SRC1:%=$(SRC1_DIR)/%)	

DEBUG_OBJS = \
$(SRC:%.c=$(DEBUG_BUILD_DIR)/%.o) \
$(SRC1:%.c=$(DEBUG_BUILD_DIR)/%.o)

RELEASE_OBJS = \
$(SRC:%.c=$(RELEASE_BUILD_DIR)/%.o) \
$(SRC1:%.c=$(RELEASE_BUILD_DIR)/%.o)

#
# Rules
#

$(DEBUG_BUILD_DIR):
	mkdir -p $@

$(RELEASE_BUILD_DIR):
	mkdir -p $@


#
# Build static library
#

ifdef DEBUG_LIB

debug: $(DEBUG_BUILD_DIR) $(DEBUG_LIB) 

$(DEBUG_LIB): $(DEBUG_OBJS)
	$(AR) $(ARFLAGS) $@ $?
	ranlib $@

endif # DEBUG_LIB

ifdef RELEASE_LIB

release: $(RELEASE_BUILD_DIR) $(RELEASE_LIB) 

$(RELEASE_LIB): $(RELEASE_OBJS)
	$(AR) $(ARFLAGS) $@ $?
	ranlib $@

endif # RELEASE_LIB

#
# Build shared library
#

ifeq ($(PLATFORM),solaris)
LD_SHARED_OPT = -G
LD_PLATFORM_LIBS = -lsocket -lnsl -lrt
else
LD_SHARED_OPT = -shared
endif

ifdef DEBUG_SO

debug: $(DEBUG_BUILD_DIR) $(DEBUG_SO) 

$(DEBUG_SO): $(DEBUG_OBJS)
	$(LD) $(LD_SHARED_OPT) $(LDFLAGS) $(LD_DEBUG_FLAGS) -o $@ $^ $(LD_DEBUG_LIBS) $(LD_PLATFORM_LIBS)

endif # DEBUG_SO

ifdef RELEASE_SO

release: $(RELEASE_BUILD_DIR) $(RELEASE_SO) 

$(RELEASE_SO): $(RELEASE_OBJ)
	$(LD) $(LD_SHARED_OPT) $(LDFLAGS) $(LD_RELEASE_FLAGS) -o $@ $^ $(LD_RELEASE_LIBS) $(LD_PLATFORM_LIBS)

$(RELEASE_SO): $(RELEASE_OBJS)

endif # RELEASE_SO

#
# Pattern Rules
#

CPPFLAGS = $(INCLUDES) $(DEFINES)
DEBUG_OPTS = -c $(DEBUG_FLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_DEFINES) 
RELEASE_OPTS = -c $(RELEASE_FLAGS) $(CFLAGS) $(CPPFLAGS) $(RELEASE_DEFINES)

$(DEBUG_BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(DEBUG_OPTS) $< -o $@

$(RELEASE_BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(RELEASE_OPTS) $< -o $@

$(DEBUG_BUILD_DIR)/%.o : $(SRC1_DIR)/%.c
	$(CC) $(DEBUG_OPTS) $< -o $@

$(RELEASE_BUILD_DIR)/%.o : $(SRC1_DIR)/%.c
	$(CC) $(RELEASE_OPTS) $< -o $@

#
# Special target (dependency rules)
#

DEPENDS = Makefile.dep

nodepend:
	-rm -fr $(DEPENDS)

veryclean: clean nodepend

depend: $(DEPENDS)

$(DEPENDS): $(SRC_FILES)
	@echo "$(MAKE): Updating dependencies"
	@echo "# -*- Mode: makefile -*-" > $(DEPENDS)
	@echo "# This file is generated automatically." >> $(DEPENDS)
	@echo "# Run 'make veryclean' and  'make' to update it." >> $(DEPENDS)
ifdef RB_HOME
	@$(CC) -MM $(CPPFLAGS) $(DEBUG_DEFINES) $^ | sed \
-e "s/^[^ ]/$(BUILD_DIR)\\/debug\\/&/g" \
-e 's/'`echo $(SLIB_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(SLIB_DIR)/g' \
-e 's/'`echo $(EXPAT_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(EXPAT_DIR)/g' \
-e 's/'`echo $(XREG_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(XREG_DIR)/g' \
-e 's/'`echo $(XRPC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(XRPC_DIR)/g' \
-e 's/'`echo $(UCS2_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(UCS2_DIR)/g' \
-e 's/'`echo $(HCODEC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(HCODEC_DIR)/g' \
-e 's/'`echo $(JNILIB_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(JNILIB_DIR)/g' \
-e 's/'`echo $(WMLENC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(WMLENC_DIR)/g' \
-e 's/'`echo $(NETSIM_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(NETSIM_DIR)/g' \
-e 's/'`echo $(RB_TEMPEST)|sed 's/[\/\.]/\\\\&/g'`'/$$(RB_TEMPEST)/g' \
-e 's/'`echo $(RB_WAE)|sed 's/[\/\.]/\\\\&/g'`'/$$(RB_WAE)/g' \
-e 's/'`echo $(RB_WPS)|sed 's/[\/\.]/\\\\&/g'`'/$$(RB_WPS)/g' \
>> $(DEPENDS)
	@$(CC) -MM $(CPPFLAGS) $(RELEASE_DEFINES) $^ | sed \
-e "s/^[^ ]/$(BUILD_DIR)\\/release\\/&/g" \
-e 's/'`echo $(SLIB_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(SLIB_DIR)/g' \
-e 's/'`echo $(EXPAT_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(EXPAT_DIR)/g' \
-e 's/'`echo $(XREG_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(XREG_DIR)/g' \
-e 's/'`echo $(XRPC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(XRPC_DIR)/g' \
-e 's/'`echo $(UCS2_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(UCS2_DIR)/g' \
-e 's/'`echo $(HCODEC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(HCODEC_DIR)/g' \
-e 's/'`echo $(JNILIB_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(JNILIB_DIR)/g' \
-e 's/'`echo $(WMLENC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(WMLENC_DIR)/g' \
-e 's/'`echo $(NETSIM_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(NETSIM_DIR)/g' \
-e 's/'`echo $(RB_TEMPEST)|sed 's/[\/\.]/\\\\&/g'`'/$$(RB_TEMPEST)/g' \
-e 's/'`echo $(RB_WAE)|sed 's/[\/\.]/\\\\&/g'`'/$$(RB_WAE)/g' \
-e 's/'`echo $(RB_WPS)|sed 's/[\/\.]/\\\\&/g'`'/$$(RB_WPS)/g' \
>> $(DEPENDS)
else # RB_HOME
	@$(CC) -MM $(CPPFLAGS) $(DEBUG_DEFINES) $^ | sed \
-e "s/^[^ ]/$(BUILD_DIR)\\/debug\\/&/g" \
-e 's/'`echo $(SLIB_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(SLIB_DIR)/g' \
-e 's/'`echo $(EXPAT_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(EXPAT_DIR)/g' \
-e 's/'`echo $(XREG_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(XREG_DIR)/g' \
-e 's/'`echo $(XRPC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(XRPC_DIR)/g' \
-e 's/'`echo $(UCS2_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(UCS2_DIR)/g' \
-e 's/'`echo $(HCODEC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(HCODEC_DIR)/g' \
-e 's/'`echo $(JNILIB_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(JNILIB_DIR)/g' \
-e 's/'`echo $(WMLENC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(WMLENC_DIR)/g' \
-e 's/'`echo $(NETSIM_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(NETSIM_DIR)/g' \
>> $(DEPENDS)
	@$(CC) -MM $(CPPFLAGS) $(RELEASE_DEFINES) $^ | sed \
-e "s/^[^ ]/$(BUILD_DIR)\\/release\\/&/g" \
-e 's/'`echo $(SLIB_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(SLIB_DIR)/g' \
-e 's/'`echo $(EXPAT_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(EXPAT_DIR)/g' \
-e 's/'`echo $(XREG_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(XREG_DIR)/g' \
-e 's/'`echo $(XRPC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(XRPC_DIR)/g' \
-e 's/'`echo $(UCS2_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(UCS2_DIR)/g' \
-e 's/'`echo $(HCODEC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(HCODEC_DIR)/g' \
-e 's/'`echo $(JNILIB_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(JNILIB_DIR)/g' \
-e 's/'`echo $(WMLENC_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(WMLENC_DIR)/g' \
-e 's/'`echo $(NETSIM_DIR)|sed 's/[\/\.]/\\\\&/g'`'/$$(NETSIM_DIR)/g' \
>> $(DEPENDS)
endif # RB_HOME
