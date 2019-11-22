//R.obtained=FN_PROTOTYPE(nan)(P1.input)
//const char *, double, ExcFlags
//P1 R E
//const char *|char,unsigned long long int|double, ExcFlags|ExcFlags
//R.V3[j] = amd_ref_nan(P1.V3[j])
//constchar* double ExcFlags
//ULP amd_ref_nan_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

const static struct data input[] =
{

    {"",                0x7ff8000000000000},
    {"456789",          0x7ff800000006f855},
    {"456789a",         0x7ff8000000000000},
    {"0x456789",        0x7ff8000000456789},
    {"0x456789g",       0x7ff8000000000000},
    {"04567123",        0x7ff800000012ee53},
    {"045671238",       0x7ff8000000000000},
    {"45.67",           0x7ff8000000000000},
    {"-4567",           0x7ff8000000000000},
    {"4567e1",          0x7ff8000000000000},
    {"0x345678912345e1",0x7ffc5678912345e1},
    {"0x4567p1",        0x7ff8000000000000},
    {NULL,              0x7ff8000000000000},


};

