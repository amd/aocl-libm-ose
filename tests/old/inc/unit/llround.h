//R.obtained=FN_PROTOTYPE(llround)(P1.input)
//double, long long int, ExcFlags
//P1 R E
//unsigned long long int|double,unsigned long long int|long long int, ExcFlags|ExcFlags
//R.V3[j] = llround(P1.V3[j])
//double, long long int, ExcFlags
//ULP amd_ref_llround_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{


{F64_POS_SNAN,0X8000000000000000LL,EXC_CHK_INVL},
    {F64_NEG_SNAN,0X8000000000000000LL,EXC_CHK_INVL},
    {F64_POS_QNAN,0X8000000000000000LL,EXC_CHK_NONE},
    {F64_NEG_QNAN,0X8000000000000000LL,EXC_CHK_NONE},
    {F64_POS_INF,0X8000000000000000LL,EXC_CHK_NONE},
    {F64_NEG_INF,0X8000000000000000LL,EXC_CHK_NONE},
    {F64_POS_ONE,0X0000000000000001LL,EXC_CHK_NONE},
    {F64_NEG_ONE,0XffffffffffffffffLL,EXC_CHK_NONE},
    {F64_POS_ZERO,0X0000000000000000LL,EXC_CHK_NONE},
    {F64_NEG_ZERO,0X0000000000000000LL,EXC_CHK_NONE},


    {0X7ff0000000000000LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0Xfff0000000000000LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X7ffc000000000000LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X7ff4000000000000LL,0X8000000000000000LL,EXC_CHK_INVL},
    {0X0LL,0X0LL,EXC_CHK_NONE},
    {0X8000000000000000LL,0X0LL,EXC_CHK_NONE},
    {0XffefffffffffffffLL,0X8000000000000000LL,EXC_CHK_NONE},
    {0Xc0862c4379671324LL,0Xfffffffffffffd3aLL,EXC_CHK_NONE},
    {0X3e45798ee2308c3aLL,0X0LL,EXC_CHK_NONE},
    {0X3fb999999999999aLL,0X0LL,EXC_CHK_NONE},
    {0X3fe0000000000000LL,0X1LL,EXC_CHK_NONE},
    {0X3feccccccccccccdLL,0X1LL,EXC_CHK_NONE},
    {0X3ff199999999999aLL,0X1LL,EXC_CHK_NONE},
    {0X3ff8000000000000LL,0X2LL,EXC_CHK_NONE},
    {0X3ffe666666666666LL,0X2LL,EXC_CHK_NONE},
    {0Xbe45798ee2308c3aLL,0X0LL,EXC_CHK_NONE},
    {0Xbfb999999999999aLL,0X0LL,EXC_CHK_NONE},
    {0Xbfe0000000000000LL,0XffffffffffffffffLL,EXC_CHK_NONE},
    {0XbfeccccccccccccdLL,0XffffffffffffffffLL,EXC_CHK_NONE},
    {0Xbff199999999999aLL,0XffffffffffffffffLL,EXC_CHK_NONE},
    {0Xbff8000000000000LL,0XfffffffffffffffeLL,EXC_CHK_NONE},
    {0Xbffe666666666666LL,0XfffffffffffffffeLL,EXC_CHK_NONE},
    {0Xbff0000000000000LL,0XffffffffffffffffLL,EXC_CHK_NONE},
    {0X4024000000fff000LL,0XaLL,EXC_CHK_NONE},
    {0X408f4fff00000000LL,0X3eaLL,EXC_CHK_NONE},
    {0X408f4c0000000000LL,0X3eaLL,EXC_CHK_NONE},
    {0X409003999999999aLL,0X401LL,EXC_CHK_NONE},
    {0X42cf000000000000LL,0X3e0000000000LL,EXC_CHK_NONE},
    {0X42c80000000fffffLL,0X300000002000LL,EXC_CHK_NONE},
    {0X42d10237f00fffffLL,0X4408dfc04000LL,EXC_CHK_NONE},
    {0X42e1067adfc342b0LL,0X8833d6fe1a16LL,EXC_CHK_NONE},
    {0X42e1067adfc34208LL,0X8833d6fe1a10LL,EXC_CHK_NONE},
    {0X42e800000091fabfLL,0Xc00000048fd6LL,EXC_CHK_NONE},
    {0X42f8000001234fcfLL,0X18000001234fdLL,EXC_CHK_NONE},
    {0X42f80000000cccf8LL,0X180000000ccd0LL,EXC_CHK_NONE},
    {0X43010237f0ccdfffLL,0X22046fe199c00LL,EXC_CHK_NONE},
    {0X430800056891fa02LL,0X30000ad123f40LL,EXC_CHK_NONE},
    {0X43110237f00000ffLL,0X4408dfc000040LL,EXC_CHK_NONE},
    {0X43110237f00000feLL,0X4408dfc000040LL,EXC_CHK_NONE},
    {0X4321ffffffffffffLL,0X9000000000000LL,EXC_CHK_NONE},
    {0X43210237f00000ffLL,0X8811bf8000080LL,EXC_CHK_NONE},
    {0X43310237f00000ffLL,0X110237f00000ffLL,EXC_CHK_NONE},
    {0X433800056891fa00LL,0X1800056891fa00LL,EXC_CHK_NONE},
    {0X4378000000000000LL,0X180000000000000LL,EXC_CHK_NONE},
    {0X1LL,0X0LL,EXC_CHK_NONE},
    {0X5fde623545abcLL,0X0LL,EXC_CHK_NONE},
    {0XfffffffffffffLL,0X0LL,EXC_CHK_NONE},
    {0X8000000000000001LL,0X0LL,EXC_CHK_NONE},
    {0X8002344ade5def12LL,0X0LL,EXC_CHK_NONE},
    {0X800fffffffffffffLL,0X0LL,EXC_CHK_NONE},
    {0X10000000000000LL,0X0LL,EXC_CHK_NONE},
    {0X8010000000000000LL,0X0LL,EXC_CHK_NONE},
    {0Xc5812e71245acfdbLL,0X8000000000000000LL,EXC_CHK_NONE},
    {0XffefffffffffffffLL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X7ff0000000000000LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0Xfff0000000000000LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X7ff8000000000000LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X7ffe1a5701234dc3LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X7fffffffffffffffLL,0X8000000000000000LL,EXC_CHK_NONE},
    {0Xfff8000000000000LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0Xfff8000000000001LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0Xfff9123425dcba31LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0XffffffffffffffffLL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X7ff0000000000001LL,0X8000000000000000LL,EXC_CHK_INVL},
    {0X7ff5344752a0bd90LL,0X8000000000000000LL,EXC_CHK_INVL},
    {0X7ff7ffffffffffffLL,0X8000000000000000LL,EXC_CHK_INVL},
    {0Xfff0000000000001LL,0X8000000000000000LL,EXC_CHK_INVL},
    {0Xfff432438995fffaLL,0X8000000000000000LL,EXC_CHK_INVL},
    {0Xfff7ffffffffffffLL,0X8000000000000000LL,EXC_CHK_INVL},
    {0X3ff921fb54442d18LL,0X2LL,EXC_CHK_NONE},
    {0X400921fb54442d18LL,0X3LL,EXC_CHK_NONE},
    {0X401921fb54442d18LL,0X6LL,EXC_CHK_NONE},
    {0X3ffb7e151628aed3LL,0X2LL,EXC_CHK_NONE},
    {0X4005bf0a8b145769LL,0X3LL,EXC_CHK_NONE},
    {0X400dbf0a8b145769LL,0X4LL,EXC_CHK_NONE},
    {0X0LL,0X0LL,EXC_CHK_NONE},
    {0X3c4536b8b14b676cLL,0X0LL,EXC_CHK_NONE},
    {0X3fdffffbce4217d3LL,0X0LL,EXC_CHK_NONE},
    {0X3fe000000000006cLL,0X1LL,EXC_CHK_NONE},
    {0X8000000000000000LL,0X0LL,EXC_CHK_NONE},
    {0Xbbdb2752ce74ff42LL,0X0LL,EXC_CHK_NONE},
    {0Xbfdffffbce4217d3LL,0X0LL,EXC_CHK_NONE},
    {0Xbfe000000000006cLL,0XffffffffffffffffLL,EXC_CHK_NONE},
    {0X3ff0000000000000LL,0X1LL,EXC_CHK_NONE},
    {0X3fefffffc49bd0dcLL,0X1LL,EXC_CHK_NONE},
    {0X3ff0000000000119LL,0X1LL,EXC_CHK_NONE},
    {0X3ff7fffef39085f4LL,0X1LL,EXC_CHK_NONE},
    {0X3ff8000000000001LL,0X2LL,EXC_CHK_NONE},
    {0Xbff0000000000000LL,0XffffffffffffffffLL,EXC_CHK_NONE},
    {0Xbfefffffc49bd0dcLL,0XffffffffffffffffLL,EXC_CHK_NONE},
    {0Xbff0000000000119LL,0XffffffffffffffffLL,EXC_CHK_NONE},
    {0Xbff7fffef39085f4LL,0XffffffffffffffffLL,EXC_CHK_NONE},
    {0Xbff8000000000001LL,0XfffffffffffffffeLL,EXC_CHK_NONE},
    {0X4000000000000000LL,0X2LL,EXC_CHK_NONE},
    {0Xc000000000000000LL,0XfffffffffffffffeLL,EXC_CHK_NONE},
    {0X4024000000000000LL,0XaLL,EXC_CHK_NONE},
    {0Xc024000000000000LL,0Xfffffffffffffff6LL,EXC_CHK_NONE},
    {0X408f400000000000LL,0X3e8LL,EXC_CHK_NONE},
    {0Xc08f400000000000LL,0Xfffffffffffffc18LL,EXC_CHK_NONE},
    {0X4050d9999999999aLL,0X43LL,EXC_CHK_NONE},
    {0Xc050d9999999999aLL,0XffffffffffffffbdLL,EXC_CHK_NONE},
    {0X409efe6666666666LL,0X7c0LL,EXC_CHK_NONE},
    {0Xc09efe6666666666LL,0Xfffffffffffff840LL,EXC_CHK_NONE},
    {0X4055e00000000000LL,0X58LL,EXC_CHK_NONE},
    {0Xc055e00000000000LL,0Xffffffffffffffa8LL,EXC_CHK_NONE},
    {0X41002bc800000000LL,0X20579LL,EXC_CHK_NONE},
    {0Xc1002bc800000000LL,0Xfffffffffffdfa87LL,EXC_CHK_NONE},
    {0X4330000000000000LL,0X10000000000000LL,EXC_CHK_NONE},
    {0X4330000000000001LL,0X10000000000001LL,EXC_CHK_NONE},
    {0X432fffffffffffffLL,0X10000000000000LL,EXC_CHK_NONE},
    {0Xc330000000000000LL,0Xfff0000000000000LL,EXC_CHK_NONE},
    {0Xc330000000000001LL,0XffefffffffffffffLL,EXC_CHK_NONE},
    {0Xc32fffffffffffffLL,0Xfff0000000000000LL,EXC_CHK_NONE},
/* Following inputs are creating problems.
 * Basically the GCC implementation is not correct in this case
 * The current AMD implementation is coming correct with the 64
 * bit taken in consideration  I*/
    /*Input Num        LL,The GCC output*/
    {0X43480677adfcfa00LL,0X00300cef5bf9f400LL,EXC_CHK_NONE}, //GCC rreturns 300cee5bf9f400LL
    {0X435801239871fa00LL,0X0060048e61c7e800LL,EXC_CHK_NONE}, //GCC rreturns 0X60048c61c7e800LL
    {0X4368111144444444LL,0X00c0888a22222220LL,EXC_CHK_NONE}, //0Xc0888822222220LL
    {0X4388ffffffffffffLL,0X031fffffffffffe0LL,EXC_CHK_NONE},//
    {0X4398ffffffffffffLL,0X063fffffffffffc0LL,EXC_CHK_NONE}, // 0x63fffc0ffffffc0
    {0X43a8ffffffffffffLL,0X0c7fffffffffff80LL,EXC_CHK_NONE},//0xc7fff80ffffff80
    {0X43b8aaaa68ffffffLL,0X18aaaa68ffffff00LL ,EXC_CHK_NONE},//18aaaa00ffffff00LL
    {0X43c8aaaa69ffffffLL,0X315554d3fffffe00LL ,EXC_CHK_NONE},//31555400fffffe00LL
    {0X43d8009678abcd00LL,0X600259e2af340000LL,EXC_CHK_NONE},//0X60025800af340000LL
    {0X7fefffffffffffffLL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X43e8098734209870LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X43f800876abcda00LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X44080091781cdba0LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X4408012786abcde0LL,0X8000000000000000LL,EXC_CHK_NONE},
    {0X43b3c4eafedcab02LL,0X13c4eafedcab0200LL,EXC_CHK_NONE},//13c4ea00dcab0200LL

};
