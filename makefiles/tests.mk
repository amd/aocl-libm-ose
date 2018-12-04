# Copyright (C) 2018, Advanced Micro Devices. All rights reserved
#
# Author: Prem Mallappa <pmallapp@amd.com>
 
TEST_MAKEFILES		:=	$(wildcard tests/*/test.mk)

include $(MK)/macros.mk

#$(info TEST_MAKEFILES=$(TEST_MAKEFILES))

ifneq ($(TEST_ONLY),)
ALL_TEST_MAKEFILES	=	$(filter tests/$(TEST_ONLY)/test.mk,$(TEST_MAKEFILES))
else
ALL_TEST_MAKEFILES	:=	$(TEST_MAKEFILES)
endif

#$(info ALL_TEST_MAKEFILES=$(ALL_TEST_MAKEFILES))

ALL_TEST_DIRS		:=	$(wildcard tests/*)


ACTIONS			:=	$(addprefix build~,$(ALL_TEST_MAKEFILES))
TESTS_CLEAN_ACTIONS	:=	$(addprefix clean~,$(ALL_TEST_MAKEFILES))
TESTS_RUN_ACTIONS	:=	$(addprefix run~,$(ALL_TEST_MAKEFILES))

ACTIONS			+=	$(TESTS_CLEAN_ACTIONS) $(TESTS_RUN_ACTIONS)
#$(info ACTIONS=$(ACTIONS))

export TESTSDIR		:=	$(BUILDDIR)/tests

$(ACTIONS):	$(ALL_TEST_MAKEFILES)
$(ACTIONS):	spec		= $(subst ~, ,$@)
$(ACTIONS):	action		= $(word 1,$(spec))
$(ACTIONS):	TEST_MAKEFILE	= $(word 2,$(spec))
$(ACTIONS):
	$(_v)$(MAKE) -f $(MK)/test.mk TEST_MAKEFILE="$(TEST_MAKEFILE)" #$(action)

RUN_TESTBINS	:=
BUILD_TESTBINS	:=

define template
include $$(TEST_MAKEFILE)
TESTBIN		:= $$(TESTSDIR)/test-$$(TEST_NAME)

BUILD_TESTBINS	+= $$(TESTBIN)
RUN_TESTBINS	+= $$(TESTBIN)

$$(TESTBIN):	build~$$(TEST_MAKEFILES)
endef

$(foreach TEST_MAKEFILE,$(ALL_TEST_MAKEFILES),$(eval $(template)))

#$(info TESTSDIR=$(TESTSDIR) TESTBIN=$(TESTBIN))
#$(info RUN_TESTBINS=$(RUN_TESTBINS) TESTBIN=$(TESTBIN))
#$(info BUILD_TESTBINS=$(BUILD_TESTBINS) $(TEST_OBJS))

.PHONY: build
build:		$(BUILD_TESTBINS)

.PHONY: run
run: $(RUN_TESTBINS)
	$(_v) ./$<
	
.PHONY: clean
clean: $(TESTS_CLEAN_ACTIONS)


