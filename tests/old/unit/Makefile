ALL_SRCS = $(wildcard *.cpp)

ALL_EXES = $(addsuffix .exe,$(basename $(ALL_SRCS)))

LINK_TYPE := 
LINK_CMD  := -L. -lamdlibm -L../ref -lRef64 -lRef32

#CFLAGS  := -I. -I../inc -I../inc/unit  -I../../../include
CFLAGS  := -I. -I../inc -I../inc/unit -I../../../include

all:       depend link $(ALL_EXES)

.PHONY: link
link:
	ln -sf ../../../build/aocl-release/src/libamdlibm.so

depend:    $(ALL_SRCS)
	g++ $(CFLAGS) -MM $^ > $@

include depend

%.exe:%.o
	g++ -O0 $(LINK_TYPE) -o $@ $< $(LINK_CMD)

%.o:%.cpp
	g++ -O0 $(CFLAGS) -ggdb -o $@ -c $< 

clean:
	rm -f *.o $(ALL_EXES) depend

