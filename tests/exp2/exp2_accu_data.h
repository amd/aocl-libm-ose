/* 
 * Copyright (C) 2018, AMD. All rights reserved
 * 
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

/*
 * These values are taken from running NAG tests
 */

#include <libm_tests.h>

struct libm_test_input_range accu_ranges[] =
    {
     {-2.048000000000e+03, -1.024000000000e+03,},
     {-1.024000000000e+03, -5.120000000000e+02,},
     {-5.120000000000e+02, -2.560000000000e+02,},
     {-2.560000000000e+02, -1.280000000000e+02,},
     {-1.280000000000e+02, -6.400000000000e+01,},
     {-6.400000000000e+01, -3.200000000000e+01,},
     {-3.200000000000e+01, -1.600000000000e+01,},
     {-1.600000000000e+01, -8.000000000000e+00,},
     {-8.000000000000e+00, -4.000000000000e+00,},
     {-4.000000000000e+00, -2.000000000000e+00,},
     {-2.000000000000e+00, -1.000000000000e+00,},
     {-1.000000000000e+00, -5.000000000000e-01,},
     {-5.000000000000e-01, -2.500000000000e-01,},
     {-2.500000000000e-01, -1.250000000000e-01,},
     {-1.250000000000e-01, -6.250000000000e-02,},
     {-6.250000000000e-02, -3.125000000000e-02,},
     {-3.125000000000e-02, -1.562500000000e-02,},
     {-1.562500000000e-02, -7.812500000000e-03,},
     {-7.812500000000e-03, -3.906250000000e-03,},
     {-3.906250000000e-03, -1.953125000000e-03,},
     {-1.953125000000e-03, -9.765625000000e-04,},
     {-9.765625000000e-04, -4.882812500000e-04,},
     {-4.882812500000e-04, -2.441406250000e-04,},
     {-2.441406250000e-04, -1.220703125000e-04,},
     {-1.220703125000e-04, -6.103515625000e-05,},
     {-6.103515625000e-05, -3.051757812500e-05,},
     {-3.051757812500e-05, -1.525878906250e-05,},
     {-1.525878906250e-05, -7.629394531250e-06,},
     {-7.629394531250e-06, -3.814697265625e-06,},
     {-3.814697265625e-06, -1.907348632812e-06,},
     {-1.907348632812e-06, -9.536743164062e-07,},
     {-9.536743164062e-07, -4.768371582031e-07,},
     {-4.768371582031e-07, -2.384185791016e-07,},
     {-2.384185791016e-07, -2.225073858507e-308,},
     {-2.225073858507e-308, -4.940656458412e-324,},
     {4.940656458412e-324, 2.225073858507e-308,},
     {2.225073858507e-308, 2.384185791016e-07,},
     {2.384185791016e-07, 4.768371582031e-07,},
     {4.768371582031e-07, 9.536743164062e-07,},
     {9.536743164062e-07, 1.907348632812e-06,},
     {1.907348632812e-06, 3.814697265625e-06,},
     {3.814697265625e-06, 7.629394531250e-06,},
     {7.629394531250e-06, 1.525878906250e-05,},
     {1.525878906250e-05, 3.051757812500e-05,},
     {3.051757812500e-05, 6.103515625000e-05,},
     {6.103515625000e-05, 1.220703125000e-04,},
     {1.220703125000e-04, 2.441406250000e-04,},
     {2.441406250000e-04, 4.882812500000e-04,},
     {4.882812500000e-04, 9.765625000000e-04,},
     {9.765625000000e-04, 1.953125000000e-03,},
     {1.953125000000e-03, 3.906250000000e-03,},
     {3.906250000000e-03, 7.812500000000e-03,},
     {7.812500000000e-03, 1.562500000000e-02,},
     {1.562500000000e-02, 3.125000000000e-02,},
     {3.125000000000e-02, 6.250000000000e-02,},
     {6.250000000000e-02, 1.250000000000e-01,},
     {1.250000000000e-01, 2.500000000000e-01,},
     {2.500000000000e-01, 5.000000000000e-01,},
     {5.000000000000e-01, 1.000000000000e+00,},
     {1.000000000000e+00, 2.000000000000e+00,},
     {2.000000000000e+00, 4.000000000000e+00,},
     {4.000000000000e+00, 8.000000000000e+00,},
     {8.000000000000e+00, 1.600000000000e+01,},
     {1.600000000000e+01, 3.200000000000e+01,},
     {3.200000000000e+01, 6.400000000000e+01,},
     {6.400000000000e+01, 1.280000000000e+02,},
     {1.280000000000e+02, 2.560000000000e+02,},
     {2.560000000000e+02, 5.120000000000e+02,},
     {5.120000000000e+02, 1.024000000000e+03,},
     {1.024000000000e+03, 2.048000000000e+03,},
     {0,},
    };
