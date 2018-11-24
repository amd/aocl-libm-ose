# Copyright (C) 2018, Advanced Micro Devices, All rights reserved.
#
# Author : Prem Mallappa <pmallapp@amd.com>


ALL_OBJS                :=      $(call TOBUILDDIR,$(ALL_OBJS))

ALL_DEPS                +=      $(ALL_OBJS:%o=%d)

ALL_STATICLIBS          +=      $(PREBUILT_STATICLIBS)

_CFLAGS			+= 	$(GLOBAL_CFLAGS) $(LIBRARY_CFLAGS)

_INCLUDES		= 	$(addprefix -I,$(GLOBAL_INCLUDES) $(EXTERNAL_INCLUDES) $(LIBRARY_INCLUDES))

# Dump all the interesting variables used to build things with
ifeq (0,1)
$(warning _INCLUDES = $(_INCLUDES))
$(warning _CFLAGS = $(_CFLAGS))
$(warning _CPPFLAGS = $(_CPPFLAGS))
$(warning _RUNTIME_FLAGS = $(_RUNTIME_FLAGS))
$(warning LIBRARY_ALLFLAGS = $(LIBRARY_ALLFLAGS))
$(warning GLOBAL_ALLFLAGS = $(GLOBAL_ALLFLAGS))
$(warning ALL_OBJS = $(ALL_OBJS))
$(warning ALL_DEPS = $(ALL_DEPS))
$(warning ALL_OBJS = $(ALL_OBJS))
endif


ifeq ($(MAKEPHASE),libraries)
$(info LIBOBJDIR=$(LIBOBJDIR))
$(LIBOBJDIR)/%.o: %.c $(LIBRARY_SRCDEPS) $(GLOBAL_SRCDEPS)
	@$(MKDIR)
	@echo LIBCC  $@
	$(_v)$(CC) $(LIBRARY_ALLFLAGS) $(GLOBAL_ALLFLAGS) $(_CFLAGS) $(_INCLUDES) -c $< -MD -MT $@ -MF $(@:%o=%d) -o $@

$(LIBOBJDIR)/%.o: %.cpp $(LIBRARY_SRCDEPS) $(GLOBAL_SRCDEPS)
	@$(MKDIR)
	@echo LIBC++  $@
	$(_v)$(CPP) $(LIBRARY_ALLFLAGS) $(GLOBAL_ALLFLAGS) $(_CPPFLAGS) $(_INCLUDES) -c $< -MD -MT $@ -MF $(@:%o=%d) -o $@

$(LIBOBJDIR)/%.o: %.S $(LIBRARY_SRCDEPS) $(GLOBAL_SRCDEPS)
	@$(MKDIR)
	@echo LIBASM $@
	$(_v)$(CC) $(LIBRARY_ALLFLAGS) $(GLOBAL_ALLFLAGS) $(GLOBAL_ASFLAGS) $(_INCLUDES) -c $< -MD -MT $@ -MF $(@:%o=%d) -o $@

# rules for source files in the libdir
$(LIBOBJDIR)/%.o: $(LIBOBJDIR)/%.c $(LIBRARY_SRCDEPS) $(GLOBAL_SRCDEPS)
	@$(MKDIR)
	@echo LIBCC  $@
	$(_v)$(CC) $(LIBRARY_ALLFLAGS) $(GLOBAL_ALLFLAGS) $(_CFLAGS) $(_INCLUDES) -c $< -MD -MT $@ -MF $(@:%o=%d) -o $@

$(LIBOBJDIR)/%.o: $(LIBOBJDIR)/%.S $(LIBRARY_SRCDEPS) $(GLOBAL_SRCDEPS)
	@$(MKDIR)
	@echo LIBASM $@
	$(_v)$(CC) $(LIBRARY_ALLFLAGS) $(GLOBAL_ALLFLAGS) $(GLOBAL_ASFLAGS) $(_INCLUDES) -c $< -MD -MT $@ -MF $(@:%o=%d) -o $@

# a rule for copying precompiled object files to somewhere they can be found at link time
$(LIBOBJDIR)/%.o: %.o
	@$(MKDIR)
	@echo CP $@
	$(_v)$(CP) $< $@
endif

