#include <fenv.h>
#include <libm_tests.h>

/* Test cases to check for exceptions for the atan() routine. These test cases are not exhaustive */
static struct __libm_test_conformance_test_data_double libm_test_atan_conformance_data[] = {
    {0x0000000000000000, 0x0000000000000000, 0},	//0
    {0x8000000000000000, 0x8000000000000000, 0},    //-0
    {0x3FF0000000000000, 0x3fe921fb54442d18, 32},    //1
    {0xbff0000000000000, 0xbfe921fb54442d18, 32},    //-1
    {0x7ff0000000000000, 0x3ff921fb54442d18, 0},   //+inf
    {0xFFF0000000000000, 0xbff921fb54442d18, 0}, //-inf
    {0x3fe279a6b50b0f28, 0x3fe0c151cbc9979a, 32}, //sqrt(3)/3
    {0x3FFBB6AE7D566CF4, 0x3ff0c15f1d5afb15, 32},  //sqrt(3) 
    {0x7ff0000000000001, 0x7ff0000000000001, 1}, //nan, nan
    {0x7ff8000000000001, 0x7ff8000000000001, 0}, //qnan, qnan
    {0xfff0000000000001, 0xfff8000000000001, 1}, //-nan, -nan
    {0xfff8000000000001, 0xfff8000000000001, 0} //-qnan, -qnan
};

/* Test cases to check for exceptions for the atanf() routine. These test cases are not exhaustive */
static struct __libm_test_conformance_test_data_float libm_test_atanf_conformance_data[] = {
    {0x00000000, 0x00000000, 0},    //atanf(0) is 0
    {0x3f800000, 0x3f490fdb, 32},   //atanf(1) = 0.785398 rads
    {0x3f13cd36, 0x3f060a8f, 32},   //atanf(sqrt(3)/3)  == 30 deg
    {0x3fddb574, 0x3f860af9, 32},    //atanf(sqrt(3) == 60 deg
    {0x7f800000, 0x3fc90fdb, 32},    //atanf(inf) = pi/2
    {0xff800000, 0xbfc90fdb, 32},    //atanf(-inf) = -pi/2
    {0x7fadffff, 0x7fedffff, 1},    //+nan
    {0x7fedffff, 0x7fedffff, 0},    //qnan
    {0xffedffff, 0xffedffff, 0},    //-qnan
    {0xffadffff, 0xffedffff, 1},    //-snan
    {0x80000000, 0x80000000, 0},    //-0
    {0xbf800000, 0xbf490fdb, 32},   //-1
};

/*for accu tests*/
static const struct libm_test_input_range atan_accu_ranges[] = {
     {-2.048000000000e+03, -1.075000000000e+03, 0},
     {-1.075000000000e+03, -1.073000000000e+03, 0},
     {-1.074000000000e+03, -1.024000000000e+03, 0},
     {-1.024000000000e+03, -1.023000000000e+03, 0},
     {-1.023000000000e+03, -1.000000000000e+03, 0},
     {-1.024000000000e+03, -5.120000000000e+02, 0},
     {-5.120000000000e+02, -2.560000000000e+02, 0},
     {-2.560000000000e+02, -1.280000000000e+02, 0},
     {-1.280000000000e+02, -6.400000000000e+01, 0},
     {-6.400000000000e+01, -3.200000000000e+01, 0},
     {-3.200000000000e+01, -1.600000000000e+01, 0},
     {-1.600000000000e+01, -8.000000000000e+00, 0},
     {-8.000000000000e+00, -4.000000000000e+00, 0},
     {-4.000000000000e+00, -2.000000000000e+00, 0},
     {-2.000000000000e+00, -1.000000000000e+00, 0},
     {-1.000000000000e+00, -5.000000000000e-01, 0},
     {-5.000000000000e-01, -2.500000000000e-01, 0},
     {-2.500000000000e-01, -1.250000000000e-01, 0},
     {-1.250000000000e-01, -6.250000000000e-02, 0},
     {-6.250000000000e-02, -3.125000000000e-02, 0},
     {-3.125000000000e-02, -1.562500000000e-02, 0},
     {-1.562500000000e-02, -7.812500000000e-03, 0},
     {-7.812500000000e-03, -3.906250000000e-03, 0},
     {-3.906250000000e-03, -1.953125000000e-03, 0},
     {-1.953125000000e-03, -9.765625000000e-04, 0},
     {-9.765625000000e-04, -4.882812500000e-04, 0},
     {-4.882812500000e-04, -2.441406250000e-04, 0},
     {-2.441406250000e-04, -1.220703125000e-04, 0},
     {-1.220703125000e-04, -6.103515625000e-05, 0},
     {-6.103515625000e-05, -3.051757812500e-05, 0},
     {-3.051757812500e-05, -1.525878906250e-05, 0},
     {-1.525878906250e-05, -7.629394531250e-06, 0},
     {-7.629394531250e-06, -3.814697265625e-06, 0},
     {-3.814697265625e-06, -1.907348632812e-06, 0},
     {-1.907348632812e-06, -9.536743164062e-07, 0},
     {-9.536743164062e-07, -4.768371582031e-07, 0},
     {-4.768371582031e-07, -2.384185791016e-07, 0},
     {-2.384185791016e-07, -2.225073858507e-308, 0},
     {-2.225073858507e-308, -4.940656458412e-324, 0},
     {4.940656458412e-324, 2.225073858507e-308, 0},
     {2.225073858507e-308, 2.384185791016e-07, 0},
     {2.384185791016e-07, 4.768371582031e-07, 0},
     {4.768371582031e-07, 9.536743164062e-07, 0},
     {9.536743164062e-07, 1.907348632812e-06, 0},
     {1.907348632812e-06, 3.814697265625e-06, 0},
     {3.814697265625e-06, 7.629394531250e-06, 0},
     {7.629394531250e-06, 1.525878906250e-05, 0},
     {1.525878906250e-05, 3.051757812500e-05, 0},
     {3.051757812500e-05, 6.103515625000e-05, 0},
     {6.103515625000e-05, 1.220703125000e-04, 0},
     {1.220703125000e-04, 2.441406250000e-04, 0},
     {2.441406250000e-04, 4.882812500000e-04, 0},
     {4.882812500000e-04, 9.765625000000e-04, 0},
     {9.765625000000e-04, 1.953125000000e-03, 0},
     {1.953125000000e-03, 3.906250000000e-03, 0},
     {3.906250000000e-03, 7.812500000000e-03, 0},
     {7.812500000000e-03, 1.562500000000e-02, 0},
     {1.562500000000e-02, 3.125000000000e-02, 0},
     {3.125000000000e-02, 6.250000000000e-02, 0},
     {6.250000000000e-02, 1.250000000000e-01, 0},
     {1.250000000000e-01, 2.500000000000e-01, 0},
     {2.500000000000e-01, 5.000000000000e-01, 0},
     {5.000000000000e-01, 1.000000000000e+00, 0},
     {1.000000000000e+00, 2.000000000000e+00, 0},
     {2.000000000000e+00, 4.000000000000e+00, 0},
     {4.000000000000e+00, 8.000000000000e+00, 0},
     {8.000000000000e+00, 1.600000000000e+01, 0},
     {1.600000000000e+01, 3.200000000000e+01, 0},
     {3.200000000000e+01, 6.400000000000e+01, 0},
     {6.400000000000e+01, 1.280000000000e+02, 0},
     {1.280000000000e+02, 2.560000000000e+02, 0},
     {2.560000000000e+02, 5.120000000000e+02, 0},
     {5.120000000000e+02, 1.024000000000e+03, 0},
     {1.024000000000e+03, 2.048000000000e+03, 0},
     {0, 0},
};
