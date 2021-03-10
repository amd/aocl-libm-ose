MPARITH_DIR = ./mparith
SRCS_COM = $(MPARITH_DIR)/cycles.c $(MPARITH_DIR)/mallocDebug.c \
		$(MPARITH_DIR)/mparith_c.c $(MPARITH_DIR)/mparith_f.c $(MPARITH_DIR)/nanny.c  \
            $(notdir $(wildcard ./*.c))

SRCS     = $(MPARITH_DIR)/controlword.c $(MPARITH_DIR)/statusword.c 
SRCS_F32 = $(SRCS_COM)
SRCS_F64 = $(SRCS_COM)

OBJS_F32 = $(addsuffix _f32.o,$(basename $(SRCS_F32)))
OBJS_F64 = $(addsuffix _f64.o,$(basename $(SRCS_F64)))
OBJS     = $(addsuffix .o,$(basename $(SRCS)))

#C_FLAGS_COM = -O2 -fno-strict-aliasing -Wall -W -Wstrict-prototypes -Wwrite-strings -Werror -D_GNU_SOURCE -D_ISOC99_SOURCE -DIS_64BIT
C_FLAGS_COM = -O2 -fno-strict-aliasing -Wall -W \
		-Wstrict-prototypes -Wwrite-strings \
		-D_GNU_SOURCE -D_ISOC99_SOURCE \
		-DIS_64BIT -I$(MPARITH_DIR)

C_FLAGS_F32 = $(C_FLAGS_COM) -DFLOAT
C_FLAGS_F64 = $(C_FLAGS_COM) -DDOUBLE

MPMATH_LIB_F32 = libmpmath32.so
MPMATH_LIB_F64 = libmpmath64.so

MPMATH_LIB_F32S = libmpmath32.a
MPMATH_LIB_F64S = libmpmath64.a

all:                    depend_f32 depend_f64 $(MPMATH_LIB_F32) $(MPMATH_LIB_F64) $(MPMATH_LIB_F64S) $(MPMATH_LIB_F32S)

depend_f32:             $(SRCS_F32) $(SRCS)
	gcc -MM -D_GNU_SOURCE -D_ISOC99_SOURCE -DIS_64BIT -DFLOAT -I$(MPARITH_DIR) $^ > $@

depend_f64:             $(SRCS_F64) $(SRCS)
	gcc -MM -D_GNU_SOURCE -D_ISOC99_SOURCE -DIS_64BIT -DDOUBLE -I$(MPARITH_DIR) $^ > $@

include depend_f32
include depend_f64

clean:
	rm -f $(OBJS) $(OBJS_F32) $(OBJS_F64) $(MPMATH_LIB_F32) $(MPMATH_LIB_F64) $(MPMATH_LIB_F64S) $(MPMATH_LIB_F32S)

distclean:
	rm -f depend_f32 depend_f64 $(OBJS_F32) $(OBJS_F64) $(MPMATH_LIB_F32) $(MPMATH_LIB_F64)

$(MPMATH_LIB): $(OBJS_F64) $(OBJS_F32) $(OBJS)
	gcc -Xlinker --version-script=vscript_f32 -Xlinker --version-script=vscript_f64 -shared -o $@ $^

$(MPMATH_LIB_F32):			$(OBJS_F32) $(OBJS)
	gcc -Xlinker --version-script=vscript_f32 -shared -o $@ $(OBJS_F32)

$(MPMATH_LIB_F64):			$(OBJS_F64) $(OBJS)
	gcc -Xlinker --version-script=vscript_f64 -shared -o $@ $(OBJS_F64)

$(MPMATH_LIB_F64S):
	ar rcs $@ *_f64.o $(MPARITH_DIR)/*_f64.o
$(MPMATH_LIB_F32S):
	ar rcs $@ *_f32.o $(MPARITH_DIR)/*_f32.o

%.o: %.c
	gcc -fPIC $(C_FLAGS_F32) -o $@ -c $<

%_f32.o:%.c
	gcc -fPIC $(C_FLAGS_F32) -o $@ -c $<

%_f64.o:%.c
	gcc -fPIC $(C_FLAGS_F64) -o $@ -c $<

