
SHELL=/bin/sh
BUILDROOT := $(shell (cd ..; pwd))

# Machine-dependent items (compiler names and flags etc.)
# go in this include file
include GNUmakefile.inc

# To build a deliverable, use "make deliverable=2" etc.
deliverable := -1
ifneq ($(deliverable),-1)
	DELNO := $(deliverable)
deltarg: deliverable
endif

all:
	@echo "Make has the following targets:"
	@echo "  all_exe_XXX"
	@echo "  all_summary_XXX"
	@echo "  all_pdf_XXX"
	@echo "where XXX is one of \"nat32\", \"nat64\", \"amd32\", \"amd64\"."
	@echo
	@echo "Individual files can be made using"
	@echo "  \"make test_exp_amd32.summary\" for example."

# Normally debugmode should be set to 0 to generate optimized
# code. Override with "make debugmode=1" for debugging purposes.
debugmode := 0
ifneq ($(debugmode),0)
  OPTDBG = $(CDEBUGFLAGS)
  OPTDBGAUX := $(CDEBUGFLAGS)
else
# Always compile the test programs themselves with debugmode
# so that timing loops don't get optimized away. Compile
# other source, such as multi-precision simulation code, with
# optimization though, unless debugmode is on.
  OPTDBG = $(CDBGFLAGS)
  OPTDBGAUX := $(COPTFLAGS)
endif

# Use "make useshared=1" to link and execute against shared library
# instead of static library
useshared := 0

# Set forceprecision non-zero to force x86 register arithmetic to
# be performed in target precision only (i.e. not to use extended precision)
# for all runs (by default it only happens on double amd runs).
forceprecision := 0
FORCEPRECISION :=
ifneq ($(forceprecision),0)
  CFLAGS += -DFORCEPRECISION
endif

# Set valgrind to use the valgrind memory debugger under Linux
valgrind := 0
VALGRIND :=
ifneq ($(valgrind),0)
  VALGRIND := valgrind --num-callers=8 --leak-check=yes --show-reachable=yes
endif

# Set both these two symbols blank if you don't want to use
# debugging memory allocator.
MALLOCDEBUG := mallocDebug.c
MALLOCDEBUGFLAG := -DMALLOCDEBUG
#MALLOCDEBUG :=
#MALLOCDEBUGFLAG :=

# Increasing NTESTS increases the number of test cases in the
# accuracy tests.
NTESTS := 100
NTIMES := 1000
TESTARGS := -ntests $(NTESTS) -ntimes $(NTIMES)

.SUFFIXES:
.SUFFIXES: .c .$(oext) .i .f .exe .summary .verbose .ps .pdf .r .x \
	   .gnuplot_data .gnuplot_driver .gnuplot_times

TESTSOURCES := $(wildcard test_*.c)

LIBDIR_AMD := $(shell (cd ../../../; pwd))
CFLAGS += -I$(LIBDIR_AMD)/include

# Rules for building auxiliary objects.

AUXSRC := mparith_c.c mparith_f.c nanny.c cycles.c \
	 controlword.c statusword.c $(MALLOCDEBUG)
AUXHDR := $(AUXSRC:.c=.h) tests.h precision.h
DOBJECTS := $(AUXSRC:.c=$(BITS).$(oext))
FOBJECTS := $(AUXSRC:.c=f$(BITS).$(oext))

$(DOBJECTS): %$(BITS).$(oext): %.c
	$(CC) $(CFLAGS) $(OPTDBGAUX) -c -DDOUBLE $(nameobj)$@ $<

$(FOBJECTS): %f$(BITS).$(oext): %.c
	$(CC) $(CFLAGS) $(OPTDBGAUX) -c -DFLOAT $(nameobj)$@ $<

# Rules for builds using native 64-bit or 32-bit libm library.

ifeq ($(platform),windows)
LIBM_NAT :=
else
#LIBM_NAT := -lm
LIBM_NAT := -L. -lamdlibm 
#LIBM_NAT := /home/chipf/glibc-build1/math/libm.a
endif
OMITTESTS_NAT := test_chgsign.c test_fma.c \
	 test_remainder_piby2.c
SRC_NAT := $(filter-out $(OMITTESTS_NAT),$(TESTSOURCES))
OBJ_DNAT := $(SRC_NAT:.c=_nat$(BITS).$(oext))
OBJ_FNAT := $(SRC_NAT:.c=f_nat$(BITS).$(oext))
ASM_DNAT := $(SRC_NAT:.c=_nat$(BITS).s)
ASM_FNAT := $(SRC_NAT:.c=f_nat$(BITS).s)
EXE_DNAT := $(SRC_NAT:.c=_nat$(BITS).exe)
EXE_FNAT := $(SRC_NAT:.c=f_nat$(BITS).exe)
ifeq ($(platos),linux)
# 64-bit Linux native atan2 function never returns for some arguments,
# so don't test it.
#EXE_DNAT := $(filter-out test_atan2_nat64.exe, $(EXE_DNAT))
endif
SUM_DNAT := $(EXE_DNAT:.exe=.summary)
SUM_FNAT := $(EXE_FNAT:.exe=.summary)
PS_DNAT := $(EXE_DNAT:.exe=.ps)
PS_FNAT := $(EXE_FNAT:.exe=.ps)

$(OBJ_DNAT): %_nat$(BITS).$(oext): %.c
	$(CC) $(CFLAGS) $(OPTDBG) -c -DDOUBLE -DTESTNAT$(BITS) $(nameobj)$@ $<
$(ASM_DNAT): %_nat$(BITS).s: %.c
ifeq ($(platform),windows)
	$(CC) $(CFLAGS) $(OPTDBG) -c -DDOUBLE -DTESTNAT$(BITS) /Fa$@ $<
else
	$(CC) $(CFLAGS) $(OPTDBG) -S -DDOUBLE -DTESTNAT$(BITS) $<
	$(MV) $*.s $@
endif
$(EXE_DNAT): $(DOBJECTS)
$(EXE_DNAT): %.exe: %.$(oext)
	$(LINK) $(CLINKFLAGS) $(nameexe)$@ $< \
	$(DOBJECTS) $(LIBM_NAT)

$(OBJ_FNAT): %f_nat$(BITS).$(oext): %.c
	$(CC) $(CFLAGS) $(OPTDBG) -c -DFLOAT -DTESTNAT$(BITS) $(nameobj)$@ $<
$(ASM_FNAT): %f_nat$(BITS).s: %.c
ifeq ($(platform),windows)
	$(CC) $(CFLAGS) $(OPTDBG) -c -DFLOAT -DTESTNAT$(BITS) /Fa$@ $<
else
	$(CC) $(CFLAGS) $(OPTDBG) -S -DFLOAT -DTESTNAT$(BITS) $<
	$(MV) $*.s $@
endif
$(EXE_FNAT): $(FOBJECTS)
$(EXE_FNAT): %.exe: %.$(oext)
	$(LINK) $(CLINKFLAGS) $(nameexe)$@ $< \
	$(FOBJECTS) $(LIBM_NAT)

all_exe_nat$(BITS): $(EXE_DNAT) $(EXE_FNAT)
all_summary_nat$(BITS): $(SUM_DNAT) $(SUM_FNAT)
all_asm_nat$(BITS): $(ASM_DNAT) $(ASM_FNAT)
all_pdf_nat$(BITS): $(PS_DNAT) $(PS_FNAT) psconcat.exe timestamp.exe
	echo "Operating system:" > .ident
	uname -a >> .ident
	echo "C compiler:" >> .ident
	echo "$(cversion)" >> .ident
	./timestamp.exe > timestamp.ps
	./psconcat.exe $(PS_DNAT) $(PS_FNAT) \
	  timestamp.ps > results_nat$(BITS).ps
	ps2pdf results_nat$(BITS).ps > results_nat$(BITS).pdf
	$(RM) .ident

# Rules for builds using amd 64-bit or 32-bit libm library.

LIBDIR_AMD = $(shell (cd ../../../; pwd))
ifeq ($(platform),windows)
LIBDIR_AMD := ../libm_amd
LIBM_AMD := $(LIBDIR_AMD)/libm_amd$(BITS).lib
else
$(info $(LIBM_AMD))
ifeq ($(useshared),0)
#LIBM_AMD := $(LIBDIR_AMD)/libm_amd$(BITS).a
#LIBM_AMD :=
LIBM_AMD = $(LIBDIR_AMD)/build/aocl-release/src/libamdlibm.so
else
endif
endif

ifeq ($(platform),windows)
OMITTESTS_AMD := test_pow10.c
else
OMITTESTS_AMD :=
endif

SRC_AMD := $(filter-out $(OMITTESTS_AMD),$(TESTSOURCES))
OBJ_DAMD := $(SRC_AMD:.c=_amd$(BITS).$(oext))
OBJ_FAMD := $(SRC_AMD:.c=f_amd$(BITS).$(oext))
ASM_DAMD := $(SRC_AMD:.c=_amd$(BITS).s)
ASM_FAMD := $(SRC_AMD:.c=f_amd$(BITS).s)
EXE_DAMD := $(SRC_AMD:.c=_amd$(BITS).exe)
EXE_FAMD := $(SRC_AMD:.c=f_amd$(BITS).exe)
SUM_DAMD := $(EXE_DAMD:.exe=.summary)
SUM_FAMD := $(EXE_FAMD:.exe=.summary)
PS_DAMD := $(EXE_DAMD:.exe=.ps)
PS_FAMD := $(EXE_FAMD:.exe=.ps)

$(OBJ_DAMD): %_amd$(BITS).$(oext): %.c
	$(CC) $(CFLAGS) $(PREPRO_FLAG) $(OPTDBG) -I$(LIBDIR_AMD) -c -DDOUBLE -DTESTAMD$(BITS) $(nameobj)$@ $<
$(ASM_DAMD): %_amd$(BITS).s: %.c
ifeq ($(platform),windows)
	$(CC) $(CFLAGS) $(OPTDBG) -I$(LIBDIR_AMD) -c -DDOUBLE -DTESTAMD$(BITS) /Fa$@ $<
else
	$(CC) $(CFLAGS) $(OPTDBG) -I$(LIBDIR_AMD)/include -S -DDOUBLE -DTESTAMD$(BITS) $<
	$(MV) $*.s $@
endif


$(info $(LIBM_AMD))
.PHONY: LN_LIBM_AMD
LN_LIBM_AMD: 
	ln -sf $(LIBM_AMD)

$(EXE_DAMD): $(DOBJECTS) LN_LIBM_AMD
$(EXE_DAMD): %.exe: %.$(oext)
	$(LINK) $(CLINKFLAGS) $(nameexe)$@ $< \
	$(DOBJECTS) $(LIBM_AMD) $(LIBM_NAT)
#	$(DOBJECTS) $(LIBM_NAT)

$(OBJ_FAMD): %f_amd$(BITS).$(oext): %.c
	$(CC) $(CFLAGS) $(PREPRO_FLAG) $(OPTDBG) -I$(LIBDIR_AMD)/include -c -DFLOAT -DTESTAMD$(BITS) $(nameobj)$@ $<
$(ASM_FAMD): %f_amd$(BITS).s: %.c
ifeq ($(platform),windows)
	$(CC) $(CFLAGS) $(OPTDBG) -I$(LIBDIR_AMD)/include -c -DFLOAT -DTESTAMD$(BITS) /Fa$@ $<
else
	$(CC) $(CFLAGS) $(OPTDBG) -I$(LIBDIR_AMD)/include -S -DFLOAT -DTESTAMD$(BITS) $<
	$(MV) $*.s $@
endif
$(EXE_FAMD): $(FOBJECTS) $(LIBM_AMD)
$(EXE_FAMD): %.exe: %.$(oext)
	$(LINK) $(CLINKFLAGS) $(nameexe)$@ $< \
	$(FOBJECTS) $(LIBM_AMD) $(LIBM_NAT)
#	$(FOBJECTS)  $(LIBM_NAT)

all_exe_amd$(BITS): $(EXE_DAMD) $(EXE_FAMD)
all_summary_amd$(BITS): $(SUM_DAMD) $(SUM_FAMD)
all_asm_amd$(BITS): $(ASM_DAMD) $(ASM_FAMD)
all_pdf_amd$(BITS): $(PS_DAMD) $(PS_FAMD) psconcat.exe timestamp.exe
	echo "Operating system:" > .ident
	uname -a >> .ident
	echo "C compiler:" >> .ident
	echo "$(cversion)" >> .ident
	./timestamp.exe > timestamp.ps
	./psconcat.exe $(PS_DAMD) $(PS_FAMD) \
	  timestamp.ps > results_amd$(BITS).ps
	ps2pdf results_amd$(BITS).ps > results_amd$(BITS).pdf
	$(RM) .ident

# Sets of tests

SUM_DNAT := $(EXE_DNAT:.exe=.summary)
SUM_FNAT := $(EXE_FNAT:.exe=.summary)
SUM_DAMD := $(EXE_DAMD:.exe=.summary)
SUM_FAMD := $(EXE_FAMD:.exe=.summary)
SUM_DFD  := $(EXE_DFD:.exe=.summary)
SUM_DNAG := $(EXE_DNAG:.exe=.summary)

ALLEXE := $(EXE_DNAT) $(EXE_FNAT) \
          $(EXE_DAMD) $(EXE_FAMD) \
          $(EXE_DFD) $(EXE_DNAG) psconcat.exe timestamp.exe
ALLASM := $(ASM_DNAT) $(ASM_FNAT) \
          $(ASM_DAMD) $(ASM_FAMD)

ALLOBJ := $(ALLEXE:.exe=.$(oext)) $(DOBJECTS) $(FOBJECTS)
ALLSUM := $(SUM_DNAT) $(SUM_FNAT) $(SUM_DAMD) $(SUM_FAMD) $(SUM_DFD) $(SUM_DNAG)
ALLVERB := $(ALLSUM:.summary=.verbose)
ALLPS := $(ALLSUM:.summary=.ps)
ALLGNUPLOT := $(ALLSUM:.summary=.gnuplot_data) $(ALLSUM:.summary=.gnuplot_times) \
	      $(ALLSUM:.summary=.gnuplot_driver)

all_exe: $(ALLEXE)
all_summary: $(ALLSUM)

summary.table: $(wildcard *.summary)
	../Tools/summary2table *.summary > summary.table

# Implicit rules

.c.$(oext):
	$(CC) $(CFLAGS) $(OPTDBG) $(CDEBUG) $(MALLOCDEBUGFLAG) -c $<

.c.i:
	$(CC) $(CFLAGS) $(OPTDBG) $(CDEBUG) $(MALLOCDEBUGFLAG) -E $< > $@


# For amd function tests, set LD_LIBRARY_PATH environment variable
# in case we're using shared library
$(SUM_DAMD): %.summary: %.exe
	@echo "************ Running $<"
	(export LD_LIBRARY_PATH=$(LIBDIR_AMD); $(VALGRIND) ./$< $(TESTARGS))

$(SUM_FAMD): %.summary: %.exe
	@echo "************ Running $<"
	(export LD_LIBRARY_PATH=$(LIBDIR_AMD); $(VALGRIND) ./$< $(TESTARGS))

# For other than amd, don't need LD_LIBRARY_PATH.
# Implicit rule to create a ".summary" file from a ".exe" file
.exe.summary:
	@echo "************ Running $<"
	$(VALGRIND) ./$< $(TESTARGS)

# Implicit rule to create a ".ps" file from a ".summary" file
.summary.ps:
	@echo "************ Convert results to PostScript file $*.ps"
	gnuplot $*.gnuplot_driver

# A program to concatenate gnuplot PostScript files, renumbering pages
# appropriately.
psconcat.exe: psconcat.c
	$(LINK) $(CLINKFLAGS) $< $(nameexe)$@

# A program to generate a timestamp PostScript file.
timestamp.exe: timestamp.c
	$(LINK) $(CLINKFLAGS) $< $(nameexe)$@

.PHONY: clean
clean:
	$(RM) $(ALLOBJ)
	$(RM) $(ALLASM)
	$(RM) $(ALLEXE)
	$(RM) $(ALLGNUPLOT)

.PHONY: cleanres
cleanres:
	$(RM) $(ALLSUM)
	$(RM) $(ALLVERB)
	$(RM) $(ALLPS) results*.ps results*.pdf

.PHONY: realclean
realclean: clean cleanres

