
TEST_MAKEFILES		:=	$(wildcard tests/*/tests.mk)

include $(MK)/macros.mk

ALL_TEST_DIRS		:=	$(wildcard tests/*)
ALL_TEST_MAKEFILES	:=	$(TEST_MAKEFILES)


ACTIONS			:=	$(addprefix build~,$(ALL_TEST_MAKEFILES))

export TESTSDIR		:=	$(BUILDDIR)


$(ACTIONS):	$(ALL_TEST_MAKEFILES)
$(ACTIONS):	spec		= $(subst ~, ,$@)
$(ACTIONS):	action		= $(word 1,$(spec))
$(ACTIONS):	TEST_MAKEFILE	= $(word 2,$(spec))
$(ACTIONS):
	@echo "=====> Building Test $(TEST_NAME)"
	$(MAKE) -f $(MK)/test.mk TEST_MAKEFILE="$(TEST_MAKEFILE)"

BUILD_TESTBINS	:=

define template
include $$(TEST_MAKEFILE)
TESTBIN		:= $$(TESTSDIR)/$$(TEST_NAME)/test-$$(TEST_NAME)

BUILD_TESTBINS	+= $$(TESTBIN)

$$(TESTBIN):	build~$$(TEST_MAKEFILES)
endef

$(foreach TEST_MAKEFILE,$(ALL_TEST_MAKEFILES),$(eval $(template)))

$(info TESTSDIR=$(TESTSDIR) TESTBINS=$(TESTBINS))
$(info BUILD_TESTBINS=$(BUILD_TESTBINS))

.PHONY: build
build:		$(BUILD_TESTBINS)
