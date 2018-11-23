# Copyright (C) 2018, Advanced Micro Devices, All rights reserved.
#
# Author : Prem Mallappa <pmallapp@amd.com>

# Isolate all source files
MODULE_CSRCS 	:= 	$(filter %.c,$(MODULE_SRCS))
MODULE_CPPSRCS 	:= 	$(filter %.cpp,$(MODULE_SRCS)) $(filter %.cxx,$(MODULE_SRCS)) \
			$(filter %.CPP,$(MODULE_SRCS)) $(filter %.CXX,$(MODULE_SRCS)) 
MODULE_ASMSRCS 	:= 	$(filter %.S,$(MODULE_SRCS))


MODULE_COBJS 	:= 	$(call TOMODULEDIR,$(patsubst %.c,%.c.o,$(MODULE_CSRCS)))
MODULE_CPPOBJS 	:= 	$(call TOMODULEDIR,$(patsubst %.cpp,%.cpp.o,$(MODULE_CPPSRCS)))
MODULE_ASMOBJS 	:= 	$(call TOMODULEDIR,$(patsubst %.S,%.S.o,$(MODULE_ASMSRCS)))

MODULE_OBJS 	:= 	$(MODULE_COBJS) $(MODULE_CPPOBJS) $(MODULE_ASMOBJS) $(MODULE_ASMOBJS2)


$(MODULE_COBJS): $(MODULE_BUILDDIR)/%.c.o: %.c $(MODULE_SRCDEPS)
	@$(MKDIR)
	$(call BUILDECHO, compiling $<)
	$(NOECHO)$(CC) $(GLOBAL_COMPILEFLAGS) $(GLOBAL_OPTFLAGS) \
		$(MODULE_OPTFLAGS) $(MODULE_COMPILEFLAGS) $(GLOBAL_CFLAGS) \
		$(MODULE_CFLAGS) \
		$(GLOBAL_INCLUDES) -c $< -MD -MP -MT $@ -MF $(@:%o=%d) -o $@

$(MODULE_CPPOBJS): $(MODULE_BUILDDIR)/%.cpp.o: %.cpp $(MODULE_SRCDEPS)
	@$(MKDIR)
	$(call BUILDECHO, compiling $<)
	$(NOECHO)$(CC) $(GLOBAL_COMPILEFLAGS) $(GLOBAL_OPTFLAGS) \
		$(MODULE_OPTFLAGS) $(MODULE_COMPILEFLAGS) $(GLOBAL_CPPFLAGS) \
		$(MODULE_CPPFLAGS) \
		$(GLOBAL_INCLUDES) -c $< -MD -MP -MT $@ -MF $(@:%o=%d) -o $@

$(MODULE_ASMOBJS): $(MODULE_BUILDDIR)/%.S.o: %.S $(MODULE_SRCDEPS)
	@$(MKDIR)
	$(call BUILDECHO, compiling $<)
	$(NOECHO)$(CC) $(GLOBAL_COMPILEFLAGS) $(GLOBAL_OPTFLAGS) \
		$(MODULE_OPTFLAGS) $(MODULE_COMPILEFLAGS) $(GLOBAL_ASMFLAGS) \
		$(MODULE_ASMFLAGS) \
		$(GLOBAL_INCLUDES) -c $< -MD -MP -MT $@ -MF $(@:%o=%d) -o $@


# Clear all the locally set variables
MODULE_CSRCS :=
MODULE_CPPSRCS :=
MODULE_ASMSRCS :=
MODULE_COBJS :=
MODULE_CPPOBJS :=
MODULE_ASMOBJS :=
