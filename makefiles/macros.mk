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

# conditionally echo text passed in
ifeq ($(V),)
BUILDECHO = @echo $(1)
else
BUILDECHO =
endif
