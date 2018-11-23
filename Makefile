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
$(info Building on $(BUILD_OS)...)
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
$(info Building using $(COMPILER_VERSION))
##############################

# Place where all objs will end up
export OBJROOT		?=	.

SPACE_CHECK		= $(filter ALL,$(and $(findstring $(subst ~, ,~),$(1)),$(error $(2))))
WARN_SPACE_IN_PATH	= A path contains one or more spaces, this may create problems
$(call SPACE_CHECK $(SRCROOT),$(WARN_SPACE_IN_PATH) (SRCROOT=$(SRCROOT)))

ifneq ($(TESTS),)
 override LIBM_TESTS		:=	$(strip $(LIBM_TESTS) $(TESTS))
endif

LIBM_TESTS		?=	$(filter-out $(SUPPRESSED_TESTS), $(VALID_TESTS))
MAKE_TESTS		:=	$(filter $(VALID_TESTS), $(LIBM_TESTS))
ERROR_TESTS		:= 	$(filter-out $(VALID_TESTS), $(LIBM_TESTS))

ifneq ($(ERROR_TESTS),)
$(error Unknown test(s) - $(ERROR_TESTS))
endif

export STANDARD_ACTIONS :=	build install tests
ACTION_TEMPLATE		:=	$(addprefix %-,$(BUILD_TESTS))
ACTIONS			:=	$(foreach action,$(STANDARD_ACTIONS),$(addprefix $(action)-,$(MAKE_TESTS)))

BUILD_TESTS		:=	$(addprefix build-,$(MAKE_TESTS))
$(info $(ACTIONS))

ifndef MAKEJOBS
  export MAXJOBS	:= $(shell echo $$((`nproc` * 2))) 
  export MAKEJOBS	:= --jobs=$(MAXJOBS)
endif

all:	build


build:	build-libm build-tests

build-libm:
	@echo "====> Building AMD LIBM"
	@$(MAKE) $(MAKEJOBS) -f $(MK)/libraries.mk build MAKEPHASE=libraries SRCROOT=$(SRCROOT)
	
.PHONY: tests
tests:
	@$(MAKE) $(MAKEOBJS) -f $(MK)/tests.mk SRCROOT=$(SRCROOT) build


$(BUILD_TESTS): build-libm

$(ACTIONS):	action 		= $(word 1, $(subst -, ,$@))
$(ACTIONS):	test		= $(word 2, $(subst -, ,$@))
$(ACTIONS):
	@echo "====> $(action) $(test)"
	@$(MAKE) $(MAKEJOBS) -f tests/$(test)/$(test).mk $(action) SRCROOT=$(SRCROOT) TEST=$(TEST) MAKEPHASE=$(action)

