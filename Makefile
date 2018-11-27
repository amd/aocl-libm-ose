# Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
#
# Author: Prem Mallappa <pmallapp@amd.com>

export BUILD_OS		:= $(shell uname -s| tr '[:upper:]' '[:lower:]')

export ORIGINAL_SRCROOT:= $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
export SRCROOT		?= $(ORIGINAL_SRCROOT)
export MK		:= $(SRCROOT)/makefiles
export BUILDDIR		:= $(SRCROOT)/build

# During make install all directories and files use DSTDIR
INSTALL_ROOT		:= $(DSTDIR)/usr/local

##############################
ifeq ($(filter clean prune,$(MAKECMDGOALS)),)
$(info Building on $(BUILD_OS)...)
endif
##############################
# Figure out any tests that are not present yet
VALID_TESTS		:= $(filter-out .%,$(notdir $(shell find $(ORIGINAL_SRCROOT)/tests -mindepth 1 -maxdepth 1 -type d)))

# Tests directory also has 'include' folder, remove it from the path
SUPPRESSED_TESTS	:= include lib

include $(MK)/config.mk

ifeq ($(BUILD_OS),linux)
COMPILER_VERSION	:= $(call GET_COMPILER_VERSION)
endif
##############################
ifeq ($(filter clean prune,$(MAKECMDGOALS)),)
$(info Building using $(COMPILER_VERSION))
endif
##############################

# Place where all objs will end up
export OBJROOT		?=	.

SPACE_CHECK		= $(filter ALL,$(and $(findstring $(subst ~, ,~),$(1)),$(error $(2))))
WARN_SPACE_IN_PATH	= "A path contains one or more spaces, this may create problems"

#$(call SPACE_CHECK $(SRCROOT),$(WARN_SPACE_IN_PATH) (SRCROOT=$(SRCROOT)))

export STANDARD_ACTIONS :=	build install clean
#export STANDARD_ACTIONS :=	#test clean-test
ACTION_TEMPLATE		:=	$(addprefix %-,$(BUILD_TESTS))
LIB_ACTIONS		:=	$(foreach action,$(STANDARD_ACTIONS),$(addprefix $(action)-,libm))

##############################
#$(info $(ACTIONS))
##############################
# 
# Remove the ugly Entering directory ... etc.
#
ifndef VERBOSE
  export MAKEFLAGS	+=	--no-print-directory
endif

ifndef MAKEJOBS
  export MAXJOBS	:= $(shell echo $$((`nproc` * 2))) 
  export MAKEJOBS	:= --jobs=$(MAXJOBS)
endif

all:	build

build:	build-libraries build-tests

libs: build-libraries

build-libraries:
	@echo "==== BUILDING LIBRARIES ===="
	@$(MAKE) $(MAKEJOBS) --no-print-directory \
		-f $(MK)/libraries.mk \
		MAKEPHASE=libraries \
		SRCROOT=$(SRCROOT)
	
.PHONY: build-tests tests
tests: build-tests
build-tests: build-libraries
build-tests:
	@echo "==== BUILDING TESTS ===="
	@$(MAKE) $(MAKEOBJS) -f $(MK)/tests.mk SRCROOT=$(SRCROOT) build

ifneq ($(TESTS),)
 override LIBM_TESTS	:=	$(strip $(LIBM_TESTS) $(TESTS))
endif

LIBM_TESTS		?=	$(filter-out $(SUPPRESSED_TESTS), $(VALID_TESTS))
MAKE_TESTS		:=	$(filter $(VALID_TESTS), $(LIBM_TESTS))
ERROR_TESTS		:= 	$(filter-out $(VALID_TESTS), $(LIBM_TESTS))

ifneq ($(ERROR_TESTS),)
$(error Unknown test(s) - $(ERROR_TESTS))
endif

TEST_ACTIONS		:= build clean
TEST_ACTIONS		=  $(foreach action,$(STANDARD_ACTIONS),$(addprefix $(action)-test-,$(MAKE_TESTS)))
##############################
#$(info $(TEST_ACTIONS))
##############################

$(TEST_ACTIONS)	: % : $(ACTION_TEMPLATE)

# This is to help build only one test
# invoke like build-test-<testname>
$(TEST_ACTIONS):	action 		= $(word 1, $(subst -, ,$@))
$(TEST_ACTIONS):	t		= $(word 2, $(subst -, ,$@))
$(TEST_ACTIONS):	test		= $(word 3, $(subst -, ,$@))
$(TEST_ACTIONS):	action_uppper   := $(call UPCASE, $(action))
$(TEST_ACTIONS): build-libraries
	@if [ "X$(action)" = "Xbuild" ]; then  	\
		echo -n "==== BUILDING "; 	\
	 else   				\
		echo -n "==== CLEANING "; 	\
	 fi
	@echo "TEST $(test) $(action_upper)===="
	@$(MAKE) $(MAKEJOBS) -f $(MK)/tests.mk SRCROOT=$(SRCROOT) TEST_ONLY=$(test) $(action)

.PHONY: clean prune
clean:
	$(_v)$(MAKE) $(MAKEOPTS) -f $(MK)/tests.mk clean
	$(_v)$(MAKE) $(MAKEOPTS) -f $(MK)/libraries.mk clean

prune:
	@echo "==== Pruning BUILD DIR ===="
	$(_v)rm -fr $(BUILDDIR)

ifeq ($(filter $(MAKECMDGOALS), clean),)
-include $(ALL_DEPS)
endif

