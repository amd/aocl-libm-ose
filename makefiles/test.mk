# Copyright (C) 2018, Advanced Micro Devices. All rights reserved
#
# Author: Prem Mallappa <pmallapp@amd.com>

include $(MK)/macros.mk
include $(MK)/config.mk

TEST_OBJS		:=
TEST_SUPPORT_OBJS	:=

TESTDIR			= $(TESTSDIR)

#$(info TEST_MAKEFILE=$(TEST_MAKEFILE))
include $(TEST_MAKEFILE)
#$(info TESTDIR=$(TESTDIR) TESTNAME=$(TEST_NAME))

TESTBIN			:= $(call TOTESTDIR,test-$(TEST_NAME))

COMMON_CFLAGS   :=      -funsigned-char -fno-strict-aliasing -g -O1
COMMON_CFLAGS   +=      -fstack-protector-all

COMMON_LDFLAGS  +=	-L$(BUILDDIR) -lamdlibm

CWARNFLAGS	+=	-Wno-missing-field-initializers

COMMON_INCLUDES :=      $(SRCROOT)/include $(SRCROOT)/tests/include

#
# Club all flags along with test specific ones
#
_CFLAGS		:= 	$(COMMON_CFLAGS) $(CWARNFLAGS) $(TEST_CFLAGS) 
_INCLUDES	:=	$(addprefix -I,$(TEST_INCLUDES)) $(addprefix -I,$(COMMON_INCLUDES))
_LDFLAGS	:=	$(COMMON_LDFLAGS) $(TEST_LDFLAGS)

BUILT_OBJS	:=	$(call TOBUILDDIR,$(TEST_OBJS) $(TEST_SUPPORT_OBJS))
ALL_DEPS	+=	$(BUILT_OBJS:%.o=%.d)

###########################
#$(info BUILT_OBJS=$(BUILT_OBJS))
###########################

$(TESTDIR)/%.o: _cflags = $(_CFLAGS)
$(TESTDIR)/%.o:	$(SRCROOT)/tests/%.c 
	@echo TESTCC $@
	@$(MKDIR)
	$(_v)$(CC) -c -o $@ $(_cflags) $(_INCLUDES) -DTEST_NAME="$(TEST_NAME)" -MD -MT $@ -MF $(@:%o=%d) $<

$(TESTDIR)/%.o:	$(SRCROOT)/tests/%.S
	@echo TESTASM $@
	@$(MKDIR)
	$(_v)$(CC) -c -o $@ $(_cflags) $(_INCLUDES) -MD -MT $@ -MF $(@:%o=%d) $<

$(TESTBIN):	$(BUILT_OBJS) $(GLOBAL_SRCDEPS)
	@echo  TESTLD $@
	$(_v)$(CC) $(_CFLAGS) -o $@ $(BUILT_OBJS) $(_LDFLAGS) 

BUILD_PREREQS		+= $(TESTBIN)
build:	$(BUILD_PREREQS)

###########################
#$(info MAKEPHASE=$(MAKEPHASE))
###########################
.PHONY: clean
clean:
	$(_v)rm -f $(TEST_OJBS) $(BUILT_OBJS)
