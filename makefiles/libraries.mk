# Copyright (C) 2018 Advanced Micro Devices. All rights reserved.
#
# Author: Prem Mallappa <pmallapp@amd.com>

LIBRARY_LIST	:=	src 		\
#			tests/lib/libmcr \
			tests/lib/crlibm \

LIB_MAKEFILE_LIST:=      $(addsuffix /library.mk,$(LIBRARY_LIST))

BUILD_ACTIONS	:=	$(addprefix build~,$(LIBRARY_LIST))
CLEAN_ACTIONS	:=	$(addprefix clean~,$(LIBRARY_LIST))

ACTIONS		:=	$(BUILD_ACTIONS) $(CLEAN_ACTIONS)

#$(info LIB_MAKEFILE_LIST=$(LIB_MAKEFILE_LIST) ACTIONS=$(ACTIONS))

$(ACTIONS):	$(LIBRARY_LIST)
$(ACTIONS):	spec	=	$(subst ~, ,$@)
$(ACTIONS):	action	=	$(word 1,$(spec))
$(ACTIONS):	lib	=	$(word 2,$(spec))
$(ACTIONS):
	$(MAKE) -f $(MK)/lib.mk LIB=$(lib) LIBNAME_UPPER=$(LIBNAME) MAKEPHASE=$(MAKEPHASE)

build:	$(BUILD_ACTION)

.PHONY: clean
clean:	$(CLEAN_ACTION)
