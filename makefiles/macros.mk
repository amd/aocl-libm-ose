# Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
#
# Prem Mallappa <pmallapp@amd.com>

TOBUILDDIR		= $(addprefix $(BUILDDIR)/,$(1))
TOLIBDIR		= $(addprefix $(LIBDIR)/,$(1))
TOLIBOBJDIR		= $(addprefix $(LIBOBJDIR)/,$(1))
TOTESTDIR		= $(addprefix $(TESTDIR)/,$(1))
TOMODULEDIR 		= $(addprefix $(MODULE_BUILDDIR)/,$(1))


# Find the local dir of the make file
GET_LOCAL_DIR    = $(patsubst %/,%,$(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))))

# makes sure the target dir exists
MKDIR = if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi

# Upcase, downcase, camelcase
UPCASE          =       $(shell echo $(1) | tr a-z A-Z)
DOWNCASE        =       $(shell echo $(1) | tr A-Z a-z)

SPACE := 
SPACE +=

SPACECHECK	= $(filter ALL,$(and $(findstring,$(space),$(1)), $(2)))

# conditionally echo text passed in
VERBOSE			?=

ifneq ($(VERBOSE),)
_v	=
else
#$(info VERBOSE=$(VERBOSE))
_v	= @
endif

ifndef RANLIB
RANLIB		:=	ranlib
endif
