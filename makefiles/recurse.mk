
SUBDIRS := $(foreach d,$(strip $(SUBDIRS)),$(firstword $(subst .,$(SPACE),$(d))))
# sort and filter out any modules that have already been included
SUBDIRS := $(sort $(SUBDIRS))
SUBDIRS := $(filter-out $(ALLSUBDIRS),$(SUBDIRS))

ifneq ($(SUBDIRS),)

ALLSUBDIRS += $(SUBDIRS)
ALLSUBDIRS := $(sort $(ALLSUBDIRS))
INCSUBDIRS := $(SUBDIRS)
SUBDIRS :=
include $(addsuffix /rules.mk,$(INCSUBDIRS))
include makefiles/recurse.mk

endif

