#!/bin/bash

LIST_FN="expf exp logf log log2f log2 log10f log10 \
         powf pow sinf sin cosf cos sincosf sincos cexpf cexp"

CWD=$(pwd)
BUILDDIR=${BUILDDIR:-"${CWD}/../../../build/aocl-release"}

unit_test_make_and_run()
{
LIST_T=""
for tgt in `echo $LIST_FN`
do
    LIST_T=$LIST_T"acc_fast"$tgt".exe "
done

export LD_LIBRARY_PATH=${CWD}:${BUILDDIR}/src:${LD_LIBRARY_PATH}

echo -e "\n\n\n"; make clean; echo -e "\n\n\n";
make "LINK_TYPE=$1" "LINK_CMD=-L${CWD} -lamdlibm" $LIST_T
for tgt in `echo $LIST_T`
do
    echo -e "\n********** $tgt **********\n"
    LD_LIBRARY_PATH=${CWD}:${BUILDDIR}/src ./$tgt
done
echo -e "\n\n\n"; make clean; echo -e "\n\n\n";
}

unit_tests()
{
echo -e "\n\n********** Unit Tests **********\n" 
build_base_lib

echo -e "\n\n********** Testing STATIC library **********\n" 
cp ${BUILDDIR}/src/libamdlibm.a .
unit_test_make_and_run -static
rm libamdlibm.a

export LD_LIBRARY_PATH=.
echo -e "\n\n********** Testing DYNAMIC library **********\n" 
cp ${BUILDDIR}/src/libamdlibm.so .
unit_test_make_and_run
rm libamdlibm.so

make clean
}

unit_tests
