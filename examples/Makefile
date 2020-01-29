ALL_SRCS = $(wildcard *.c)
ALL_OBJS = $(ALL_SRCS:.c=.o) 
HEADER_PATH = ../include/external
LIB_PATH = ../build/aocl-release/src
EXE = test_libm
LFLAGS = -lamdlibm -lm
CC = gcc

%.o: %.c $(HEADER_PATH)
	$(CC) -c -I$(HEADER_PATH) ${ALL_SRCS}

test_libm: $(ALL_OBJS)
	$(CC) -o $(EXE) $(ALL_OBJS) -L $(LIB_PATH) $(LFLAGS)

.PHONY:clean
clean:
	rm -fr $(ALL_OBJS) rm -fr $(EXE)
