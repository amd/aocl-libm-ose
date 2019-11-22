NAG_DIR = ../nag
SRCS_COM = $(NAG_DIR)/controlword.c $(NAG_DIR)/cycles.c $(NAG_DIR)/mallocDebug.c \
			$(NAG_DIR)/mparith_c.c $(NAG_DIR)/mparith_f.c $(NAG_DIR)/nanny.c $(NAG_DIR)/statusword.c \
            $(notdir $(wildcard ./*.c))

SRCS_F32 = $(SRCS_COM)
SRCS_F64 = $(SRCS_COM)

OBJS_F32 = $(addsuffix _f32.o,$(basename $(SRCS_F32)))
OBJS_F64 = $(addsuffix _f64.o,$(basename $(SRCS_F64)))

#C_FLAGS_COM = -O2 -fno-strict-aliasing -Wall -W -Wstrict-prototypes -Wwrite-strings -Werror -D_GNU_SOURCE -D_ISOC99_SOURCE -DIS_64BIT
C_FLAGS_COM = -O2 -fno-strict-aliasing -Wall -W -Wstrict-prototypes -Wwrite-strings -D_GNU_SOURCE -D_ISOC99_SOURCE -DIS_64BIT
C_FLAGS_F32 = $(C_FLAGS_COM) -DFLOAT
C_FLAGS_F64 = $(C_FLAGS_COM) -DDOUBLE

REF_LIB_F32 = libRef32.so
REF_LIB_F64 = libRef64.so

REF_LIB_F32S = libRef32.a
REF_LIB_F64S = libRef64.a

all:                    depend_f32 depend_f64 $(REF_LIB_F32) $(REF_LIB_F64) $(REF_LIB_F64S) $(REF_LIB_F32S)

depend_f32:             $(SRCS_F32)
	gcc -MM -D_GNU_SOURCE -D_ISOC99_SOURCE -DIS_64BIT -DFLOAT -I$(NAG_DIR) $^ > $@

depend_f64:             $(SRCS_F64)
	gcc -MM -D_GNU_SOURCE -D_ISOC99_SOURCE -DIS_64BIT -DDOUBLE -I$(NAG_DIR) $^ > $@

include depend_f32
include depend_f64

clean:
	rm -f $(OBJS_F32) $(OBJS_F64) $(REF_LIB_F32) $(REF_LIB_F64) $(REF_LIB_F64S) $(REF_LIB_F32S)

distclean:
	rm -f depend_f32 depend_f64 $(OBJS_F32) $(OBJS_F64) $(REF_LIB_F32) $(REF_LIB_F64)

$(REF_LIB_F32):			$(OBJS_F32)
	gcc -Xlinker --version-script=vscript_f32 -shared -o $@ $(OBJS_F32)

$(REF_LIB_F64):			$(OBJS_F64)
	gcc -Xlinker --version-script=vscript_f64 -shared -o $@ $(OBJS_F64)

$(REF_LIB_F64S):
	ar rcs $@ *_f64.o $(NAG_DIR)/*_f64.o
$(REF_LIB_F32S):
	ar rcs $@ *_f32.o $(NAG_DIR)/*_f32.o

%_f32.o:%.c
	gcc -fPIC $(C_FLAGS_F32) -o $@ -c $<

%_f64.o:%.c
	gcc -fPIC $(C_FLAGS_F64) -o $@ -c $<

