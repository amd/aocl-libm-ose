
# This file, used to set machine-dependent items such as
# compiler names and flags, is designed to be included
# by the libm_amd GNUmakefile

# Retrieve OS info
machine    := $(strip $(subst ' ',, $(shell uname -m)))
os         := $(strip $(subst ' ',, $(shell uname -s)))
osver      := $(strip $(subst ' ',, $(shell uname -r)))

platform := unknown
platos := unknown

ifeq ($(os),SunOS)
platform := solaris
platos   := solaris
endif
ifeq ($(machine)$(os),i686Linux)
platform := linux-i686
platos   := linux
endif
ifeq ($(machine)$(os),x86_64Linux)
platform := linux-x86_64
platos   := linux
endif
ifeq ($(os),AIX)
platform := aix
platos   := aix
endif
ifeq ($(os)$(osver),HP-UXB.11.00)
platform := hpux11.00
platos   := hpux
endif
ifeq ($(os)$(osver),OSF1V4.0)
platform := osf4.0
platos   := osf
endif
ifeq ($(os)$(osver),IRIX646.5)
platform := irix6.5
platos   := irix
endif
ifeq ($(machine)$(os),i686CYGWIN_NT-5.1)
platform := windows
platos   := windows
endif
ifeq ($(machine)$(os),i586CYGWIN_NT-5.2)
platform := windows
platos   := windows
endif

ifeq ($(platform),unknown)
$(error Unsupported operating system $(os), version $(osver))
endif

############################## linux #####################################
ifeq ($(platform),linux-i686)
MV := mv
RM := rm -f
AR := ar rv
ARX := ar xv
CP := cp
MKDIR := mkdir -p

CC := gcc
CFLAGS := -Wall -W -Wstrict-prototypes -Wwrite-strings \
          -D_GNU_SOURCE -D_ISOC99_SOURCE -Dlinux
CDEBUGFLAGS := -g
COPTFLAGS := -O
LINK := $(CC)
CLINKFLAGS :=
BITS := 32

oext := o
nameobj := -o
namelib :=
nameexe := -o

# Any system libraries that need to be linked with programs
SYSLIBS :=

# Extract version information from C compiler
cversion   := $(shell $(CC) -v 2>&1 | grep -i version)
uname      := $(shell uname -a)

endif

############################## 64 bit linux #####################################
ifeq ($(platform),linux-x86_64)
MV := mv
RM := rm -f
AR := ar rv
CP := cp
MKDIR := mkdir -p

oext := o
nameobj := -o
namelib :=

CC := gcc
CFLAGS := -Wall -W -Wstrict-prototypes -Wwrite-strings \
          -D_GNU_SOURCE -D_ISOC99_SOURCE
CDEBUGFLAGS := -g
COPTFLAGS := -O
LINK := $(CC)
CLINKFLAGS :=
BITS := 64
# Check if BITS was overridden on the make command line.
ifeq ($(BITS),32)
CFLAGS += -m32
CLINKFLAGS += -m32
else
CFLAGS += -DIS_64BIT
endif

oext := o
nameobj := -o
namelib :=
nameexe := -o

# Any system libraries that need to be linked with programs
SYSLIBS :=

# Extract version information from C compiler
cversion   := $(shell $(CC) -v 2>&1 | grep -i version)
uname      := $(shell uname -a)

endif

############################## solaris ##################################
ifeq ($(platform),solaris)
MV := mv
RM := rm -f
AR := ar rv
ARX := ar xv
CP := cp
MKDIR := mkdir -p

CC := cc
CFLAGS := -KPIC -fast -mt
CDEBUGFLAGS := -g
COPTFLAGS := -O
LINK := $(CC)
CLINKFLAGS := -mt
BITS := 32

oext := o
nameobj := -o
namelib :=
nameexe := -o

# Any system libraries that need to be linked with programs
SYSLIBS :=

# Extract version information from C compiler
cversion   := $(shell $(CC) -v 2>&1 | head -1)
uname      := $(shell uname -a)

endif

############################## hpux 11.00 ##################################
ifeq ($(platform),hpux11.00)
MV := mv
RM := rm -f
AR := ar rv
ARX := ar xv
CP := cp
MKDIR := mkdir -p

CC := cc
CFLAGS := +w1 +M -Aa +Z +DAportable -D_HPUX_SOURCE -D__STDC__
CDEBUGFLAGS := -g
COPTFLAGS := -O
LINK := $(CC)
CLINKFLAGS :=
BITS := 32

oext := o
nameobj := -o
namelib :=
nameexe := -o

# Any system libraries that need to be linked with programs
SYSLIBS :=

# Extract version information from C compiler
cversion   := $(shell $(CC) -v 2>&1 | head -1)
uname      := $(shell uname -a)

endif

############################## osf 4.0 ##################################
ifeq ($(platform),osf4.0)
MV := mv
RM := rm -f
AR := ar rv
ARX := ar xv
CP := cp
MKDIR := mkdir -p

CC := cc
CFLAGS := -std1 -w0 -warnprotos  -D_IEEE_FP -ieee_with_no_inexact -ieee
CDEBUGFLAGS := -g
COPTFLAGS := -O
LINK := $(CC)
CLINKFLAGS :=
BITS := 32

oext := o
nameobj := -o
namelib :=
nameexe := -o

# Any system libraries that need to be linked with programs
SYSLIBS :=

# Extract version information from C compiler
cversion   := $(shell $(CC) -V 2>&1 | head -1)
uname      := $(shell uname -a)

endif

############################## irix 6.5 ##################################
ifeq ($(platform),irix6.5)
MV := mv
RM := rm -f
AR := ar rv
ARX := ar xv
CP := cp
MKDIR := mkdir -p

CC := cc
CFLAGS := -n32 -mips3 -KPIC -D_POSIX_C_SOURCE=199506L -D_XOPEN_SOURCE=500
CDEBUGFLAGS := -g
COPTFLAGS := -O
LINK := $(CC)
CLINKFLAGS :=
BITS := 32

oext := o
nameobj := -o
namelib :=
nameexe := -o

# Any system libraries that need to be linked with programs
SYSLIBS :=

# Extract version information from C compiler
cversion   := $(shell $(CC) -version 2>&1 | head -1)
uname      := $(shell uname -a)

endif

############################## aix ##################################
ifeq ($(platform),aix)
MV := mv
RM := rm -f
AR := ar rv
ARX := ar xv
CP := cp
MKDIR := mkdir -p

CC := cc_r
CFLAGS := -brtl
CDEBUGFLAGS := -g -qdbextra
COPTFLAGS := -O
LINK := $(CC)
CLINKFLAGS :=
BITS := 32

oext := o
nameobj := -o
namelib :=
nameexe := -o

# Any system libraries that need to be linked with programs
SYSLIBS :=

# Extract version information from C compiler
cversion   := $(shell $(CC) -v 2>&1 | head -1)
uname      := $(shell uname -a)

endif

############################## windows #####################################
ifeq ($(platform),windows)
MV := mv
RM := rm -f
AR := lib
CP := cp
MKDIR := mkdir

oext := obj
nameobj := /Fo
namelib := /OUT:
nameexe := /OUT:

CC := cl
# Extract version information from C compiler
cversion   := $(shell $(CC) -v 2>&1 | grep -i version)
uname      := $(shell uname -a)

ifneq (,$(findstring 32-bit,$(cversion)))
# 32 bit compiler
BITS := 32
CFLAGS := -W3 -DWINDOWS
CDEBUGFLAGS := -Zi
COPTFLAGS := -O2
LINK := link
CLINKFLAGS :=
else
# 64 bit compiler
BITS := 64
CFLAGS := -W3 -DWINDOWS -DIS_64BIT
CDEBUGFLAGS := -Zi -FAsc
COPTFLAGS := -O2
LINK := link
#CLINKFLAGS := /DEBUG /MAP /FORCE:multiple /ENTRY:Clear_AC_Entry
CLINKFLAGS := /DEBUG /MAP /FORCE:multiple
endif

# The -Oi- flag disables use of "intrinsic" functions. Both
# intrinsic and non-intrinsic functions should be tested.
CFLAGS += -Oi-

# Any system libraries that need to be linked with programs
SYSLIBS :=

endif
