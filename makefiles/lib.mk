# Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
#
# Author: Prem Mallappa <pmallapp@amd.com>

include $(MK)/macros.mk
include $(MK)/config.mk

#LIBDIR		:=	$(OBJROOT)/build
LIBDIR		:=	$(BUILDDIR)
LIBOBJDIR	:=	$(addprefix $(LIBDIR)/,$(LIB))
LIBOBJDIR	:=	$(LIBDIR)

GLOBAL_ALLFLAGS	+=
LIBRARY_LDFLAGS	+=

include $(LIB)/library.mk

TARGETVARS	:= 	$(addsuffix _BUILD,$(LIB_NAME))
#$(info TARGETVARS=$(TARGETVARS))
MAKELIBS	:=	$(foreach i,$(TARGETVARS),$(value $i))
#$(info MAKELIBS=$(MAKELIBS))

#$(MAKELIBS): objs = $(value $(addsuffix _OBJS,$(basename $(notdir $@))))
$(MAKELIBS): objs = $(value $(addsuffix _OBJS,$(LIB_NAME)))
$(MAKELIBS):
	@$(MKDIR)
	@echo AR $@
	$(_v)$(AR) -crS $@ $(objs)
	$(_v)$(RANLIB) $@

include $(MK)/compile.mk

build: $(MAKELIBS)

clean:
	$(_v) rm -f $(MAKELIBS)
	rm -f $(addsuffix _BUILD,$(COMMONLIBS))
	rm -f $(addsuffix _OBJS,$(COMMONLIBS))

