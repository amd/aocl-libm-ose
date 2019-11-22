ALL_SRCS =  acc_fastexp.cpp \
            acc_fastlogf.cpp acc_fastlog2f.cpp acc_fastlog10f.cpp acc_fastlog.cpp acc_fastlog2.cpp acc_fastlog10.cpp \
            acc_fastpowf.cpp acc_pow.cpp \
            acc_fastsinf.cpp acc_fastcosf.cpp acc_fastsin.cpp acc_fastcos.cpp \
            acc_fastsincosf.cpp acc_fastsincos.cpp acc_fastcexp.cpp acc_fastcexpf.cpp acc_fastexpf.cpp
#            acc_fastexpf.cpp acc_vrs4expf.cpp acc_vrs8expf.cpp acc_vrsaexpf.cpp \
            acc_vrd2exp.cpp acc_vrd4exp.cpp acc_vrdaexp.cpp \
             acc_vrs4powf.cpp acc_vrsapowf.cpp            
ALL_EXES = $(addsuffix .exe,$(basename $(ALL_SRCS)))

all:       depend $(ALL_EXES)

depend:    $(ALL_SRCS)
	g++ -MM $^ > $@

include depend

%.exe:%.o
	g++ -O0 $(LINK_TYPE) -o $@ $< $(LINK_CMD)

%.o:%.cpp
	g++ -O0 -I../inc -ggdb -o $@ -c $< 

clean:
	rm -f *.o $(ALL_EXES) depend

