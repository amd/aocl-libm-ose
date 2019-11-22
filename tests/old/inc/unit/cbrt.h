//R.obtained=FN_PROTOTYPE(cbrt)(P1.input)
//double,double,ExcFlags
//P1 R E
//unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_cbrt(P1.V3[j])
//double double ExcFlags
//ULP amd_ref_cbrt_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
{0x7ff0000000000001,0x7ff8000000000001,EXC_CHK_INVL},
{0xfff0000000000001,0xfff8000000000001,EXC_CHK_INVL},
{0x7ff8000000000000,0x7ff8000000000000,EXC_CHK_NONE},
{0xfff8000000000000,0xfff8000000000000,EXC_CHK_NONE},
{0x7ff0000000000000,0x7ff0000000000000,EXC_CHK_NONE},
{0xfff0000000000000,0xfff0000000000000,EXC_CHK_NONE},
{0x3ff0000000000000,0x3ff0000000000000,EXC_CHK_NONE},
{0xbff0000000000000,0xbff0000000000000,EXC_CHK_NONE},
{0x0,0x0,EXC_CHK_NONE},
{0x8000000000000000,0x8000000000000000,EXC_CHK_NONE},
{0xfffffffffffff,0x2aa428a2f98d728a,EXC_CHK_NONE},
{0x800fffffffffffff,0xaaa428a2f98d728a,EXC_CHK_NONE},
{0x1,0x2990000000000000,EXC_CHK_NONE},
{0x8000000000000001,0xa990000000000000,EXC_CHK_NONE},
{0x7fefffffffffffff,0x554428a2f98d728b,EXC_CHK_NONE},
{0xffefffffffffffff,0xd54428a2f98d728b,EXC_CHK_NONE},
{0x10000000000000,0x2aa428a2f98d728b,EXC_CHK_NONE},
{0x8010000000000000,0xaaa428a2f98d728b,EXC_CHK_NONE},
{0x400921fb60000000,0x3ff76ef7ead69852,EXC_CHK_NONE},
{0xc00921fb60000000,0xbff76ef7ead69852,EXC_CHK_NONE},
{0x3ff921fb60000000,0x3ff2996267c5deee,EXC_CHK_NONE},
{0xbff921fb60000000,0xbff2996267c5deee,EXC_CHK_NONE},
{0x1ff09cff6,0x2a3ffade2d204819,EXC_CHK_NONE},
{0x3fdffffbce4217d3,0x3fe965fd8929cd67,EXC_CHK_NONE}, // (0.499999 ,0.499999)^1/3 = (0.7937 ,0.793700)
{0x8000000000000000,0x8000000000000000,EXC_CHK_NONE}, // (-0 ,-0.000000)^1/3 = (-0 ,-0.000000)
{0x1,0x2990000000000000,EXC_CHK_NONE}   , // (4.94066e-324 ,0.000000)^1/3 = (1.70318e-108 ,0.000000)
{0x5fde623545abc,0x2a9d0f87e1bd5c8a,EXC_CHK_NONE}, // (8.33261e-309 ,0.000000)^1/3 = (2.02734e-103 ,0.000000)
{0xfffffffffffff,0x2aa428a2f98d728a,EXC_CHK_NONE}, // (2.22507e-308 ,0.000000)^1/3 = (2.81264e-103 ,0.000000)
{0x8000000000000001,0xa990000000000000,EXC_CHK_NONE}, // (-4.94066e-324 ,-0.000000)^1/3 = (-1.70318e-108 ,-0.000000)
{0x8002344ade5def12,0xaa94d2aa3036cd87,EXC_CHK_NONE}, // (-3.06541e-309 ,-0.000000)^1/3 = (-1.45266e-103 ,-0.000000)
{0x88f2344ade5def12,0xad9a83d4bb116375,EXC_CHK_NONE}, // (-3.06541e-309 ,-0.000000)^1/3 = (-1.45266e-103 ,-0.000000)
{0x800fffffffffffff,0xaaa428a2f98d728a,EXC_CHK_NONE}, // (-2.22507e-308 ,-0.000000)^1/3 = (-2.81264e-103 ,-0.000000)
{0x10000000000000,0x2aa428a2f98d728b,EXC_CHK_NONE}, // (2.22507e-308 ,0.000000)^1/3 = (2.81264e-103 ,0.000000)
{0x8010000000000000,0xaaa428a2f98d728b,EXC_CHK_NONE}, // (-2.22507e-308 ,-0.000000)^1/3 = (-2.81264e-103 ,-0.000000)
{0x43f800876abcda00,0x4147139faf7044d9,EXC_CHK_NONE}, // (2.76725e+19 ,27672498390639312896.000000)^1/3 = (3.0247e+06 ,3024703.370614)
{0x44080091781cdba0,0x414d1327c8354fcc,EXC_CHK_NONE}, // (5.53454e+19 ,55345350463248924672.000000)^1/3 = (3.8109e+06 ,3810895.564127)
{0x4408012786abcde0,0x414d13645ebbb035,EXC_CHK_NONE}, // (5.53506e+19 ,55350630119951302656.000000)^1/3 = (3.81102e+06 ,3811016.740103)
{0xffefffffffffffff,0xd54428a2f98d728b,EXC_CHK_NONE}, // (-1.79769e+308 ,-179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000)^1/3 = (-5.6438e+102 ,-5643803094122362298386835960720996333881089063696129532042408160730270011663197773501163653620081623040.000000)
{0xc0862c4379671324,0xc021d69cc4f64c8e,EXC_CHK_NONE}, // (-709.533 ,-709.532946)^1/3 = (-8.91916 ,-8.919165)
{0x3e45798ee2308c3a,0x3f61a62d511f2b53,EXC_CHK_NONE}, // (1e-08 ,0.000000)^1/3 = (0.00215443 ,0.002154)
{0x3fb999999999999a,0x3fddb4c7760bcff3,EXC_CHK_NONE}, // (0.1 ,0.100000)^1/3 = (0.464159 ,0.464159)
{0x3fe0000000000000,0x3fe965fea53d6e3d,EXC_CHK_NONE}, // (0.5 ,0.500000)^1/3 = (0.793701 ,0.793701)
{0x3feccccccccccccd,0x3feee549fe7085e7,EXC_CHK_NONE}, // (0.9 ,0.900000)^1/3 = (0.965489 ,0.965489)
{0x3ffe666666666666,0x3ff3d126bbbd1b43,EXC_CHK_NONE}, // (1.9 ,1.900000)^1/3 = (1.23856 ,1.238562)
{0xbe45798ee2308c3a,0xbf61a62d511f2b53,EXC_CHK_NONE}, // (-1e-08 ,-0.000000)^1/3 = (-0.00215443 ,-0.002154)
{0xbfb999999999999a,0xbfddb4c7760bcff3,EXC_CHK_NONE}, // (-0.1 ,-0.100000)^1/3 = (-0.464159 ,-0.464159)
{0xbfe0000000000000,0xbfe965fea53d6e3d,EXC_CHK_NONE}, // (-0.5 ,-0.500000)^1/3 = (-0.793701 ,-0.793701)
{0xbfeccccccccccccd,0xbfeee549fe7085e7,EXC_CHK_NONE}, // (-0.9 ,-0.900000)^1/3 = (-0.965489 ,-0.965489)
{0xbff199999999999a,0xbff08438278323a4,EXC_CHK_NONE}, // (-1.1 ,-1.100000)^1/3 = (-1.03228 ,-1.032280)
{0xbff8000000000000,0xbff250bfe1b082f5,EXC_CHK_NONE}, // (-1.5 ,-1.500000)^1/3 = (-1.14471 ,-1.144714)
{0xbffe666666666666,0xbff3d126bbbd1b43,EXC_CHK_NONE}, // (-1.9 ,-1.900000)^1/3 = (-1.23856 ,-1.238562)
{0xbff0000000000000,0xbff0000000000000,EXC_CHK_NONE}, // (-1 ,-1.000000)^1/3 = (-1 ,-1.000000)
{0x4024000000fff000,0x40013c484181f573,EXC_CHK_NONE}, // (10 ,10.000000)^1/3 = (2.15443 ,2.154435)
{0x408f4fff00000000,0x4024036904b5157e,EXC_CHK_NONE}, // (1002 ,1001.999512)^1/3 = (10.0067 ,10.006661)
{0x408f4c0000000000,0x4024028f08580058,EXC_CHK_NONE}, // (1001.5 ,1001.500000)^1/3 = (10.005 ,10.004998)
{0x409003999999999a,0x40242a25e8c2dfa3,EXC_CHK_NONE}, // (1024.9 ,1024.900000)^1/3 = (10.0823 ,10.082320)
{0x42cf000000000000,0x40e3f24f62645865,EXC_CHK_NONE}, // (6.81697e+13 ,68169720922112.000000)^1/3 = (40850.5 ,40850.480761)
{0x42c80000000fffff,0x40e250bfe1b494e6,EXC_CHK_NONE}, // (5.27766e+13 ,52776558141439.992188)^1/3 = (37510 ,37509.996302)
{0x42d10237f00fffff,0x40e492e11d5ec942,EXC_CHK_NONE}, // (7.48049e+13 ,74804904345599.984375)^1/3 = (42135 ,42135.034835)
{0x42e1067adfc342b0,0x40e9ee042bddccf1,EXC_CHK_NONE}, // (1.49756e+14 ,149756231686677.500000)^1/3 = (53104.1 ,53104.130355)
{0x42e1067adfc34208,0x40e9ee042bddcc9c,EXC_CHK_NONE}, // (1.49756e+14 ,149756231686672.250000)^1/3 = (53104.1 ,53104.130355)
{0x42e800000091fabf,0x40ed12ed0b2c94f9,EXC_CHK_NONE}, // (2.11106e+14 ,211106232831957.968750)^1/3 = (59543.4 ,59543.407614)
{0x42f8000001234fcf,0x40f250bfe1fa9d9f,EXC_CHK_NONE}, // (4.22212e+14 ,422212466259196.937500)^1/3 = (75020 ,75019.992671)
{0x42f80000000cccf8,0x40f250bfe1b3c48e,EXC_CHK_NONE}, // (4.22212e+14 ,422212465118415.500000)^1/3 = (75020 ,75019.992603)
{0x43010237f0ccdfff,0x40f492e11daaf0d8,EXC_CHK_NONE}, // (5.98439e+14 ,598439236312063.875000)^1/3 = (84270.1 ,84270.069743)
{0x430800056891fa02,0x40f7137604d34ca5,EXC_CHK_NONE}, // (8.44428e+14 ,844427833786176.250000)^1/3 = (94519.4 ,94519.376178)
{0x43110237f00000ff,0x40f9ebda2782c2a7,EXC_CHK_NONE}, // (1.19688e+15 ,1196878469267519.750000)^1/3 = (106174 ,106173.634646)
{0x43110237f00000fe,0x40f9ebda2782c2a6,EXC_CHK_NONE}, // (1.19688e+15 ,1196878469267519.500000)^1/3 = (106174 ,106173.634646)
{0x4321ffffffffffff,0x4100a402fcc79298,EXC_CHK_NONE}, // (2.53327e+15 ,2533274790395903.500000)^1/3 = (136320 ,136320.373428)
{0x43210237f00000ff,0x410054532d8952a7,EXC_CHK_NONE}, // (2.39376e+15 ,2393756938535039.500000)^1/3 = (133770 ,133770.397235)
{0x43310237f00000ff,0x410492e11d585626,EXC_CHK_NONE}, // (4.78751e+15 ,4787513877070079.000000)^1/3 = (168540 ,168540.139329)
{0x433800056891fa00,0x4107137604d34ca4,EXC_CHK_NONE}, // (6.75542e+15 ,6755422670289408.000000)^1/3 = (189039 ,189038.752356)
{0x4378000000000000,0x411d12ed0af1a27f,EXC_CHK_NONE}, // (1.08086e+17 ,108086391056891904.000000)^1/3 = (476347 ,476347.260687)
{0xc5812e71245acfdb,0xc1ca023dfc1e8e4d,EXC_CHK_NONE}, // (-6.64674e+26 ,-664673756588022451306233856.000000)^1/3 = (-8.72709e+08 ,-872709112.238718)
{0xffefffffffffffff,0xd54428a2f98d728b,EXC_CHK_NONE}, // (-1.79769e+308 ,-179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000)^1/3 = (-5.6438e+102 ,-5643803094122362298386835960720996333881089063696129532042408160730270011663197773501163653620081623040.000000)
{0x3ff921fb54442d18,0x3ff2996264e0e3fe,EXC_CHK_NONE}, // (1.5708 ,1.570796)^1/3 = (1.16245 ,1.162447)
{0x400921fb54442d18,0x3ff76ef7e73104b7,EXC_CHK_NONE}, // (3.14159 ,3.141593)^1/3 = (1.46459 ,1.464592)
{0x401921fb54442d18,0x3ffd8639fdcb60ea,EXC_CHK_NONE}, // (6.28319 ,6.283185)^1/3 = (1.84527 ,1.845270)
{0x3ffb7e151628aed3,0x3ff329f7e9744467,EXC_CHK_NONE}, // (1.71828 ,1.718282)^1/3 = (1.19775 ,1.197746)
{0x4005bf0a8b145769,0x3ff6546db1ba2d13,EXC_CHK_NONE}, // (2.71828 ,2.718282)^1/3 = (1.39561 ,1.395612)
{0x400dbf0a8b145769,0x3ff8c99eacdcafa5,EXC_CHK_NONE}, // (3.71828 ,3.718282)^1/3 = (1.54922 ,1.549224)
{0x3c4536b8b14b676c,0x3eb625621463f313,EXC_CHK_NONE}, // (2.3e-18 ,0.000000)^1/3 = (1.32001e-06 ,0.000001)
{0x3fe000000000006c,0x3fe965fea53d6e76,EXC_CHK_NONE}, // (0.5 ,0.500000)^1/3 = (0.793701 ,0.793701)
{0x8000000000000000,0x8000000000000000,EXC_CHK_NONE}, // (-0 ,-0.000000)^1/3 = (-0 ,-0.000000)
{0xbbdb2752ce74ff42,0xbe9315b9ecc4ce0f,EXC_CHK_NONE}, // (-2.3e-20 ,-0.000000)^1/3 = (-2.84387e-07 ,-0.000000)
{0xbfdffffbce4217d3,0xbfe965fd8929cd67,EXC_CHK_NONE}, // (-0.499999 ,-0.499999)^1/3 = (-0.7937 ,-0.793700)
{0xbfe000000000006c,0xbfe965fea53d6e76,EXC_CHK_NONE}, // (-0.5 ,-0.500000)^1/3 = (-0.793701 ,-0.793701)
{0x3ff0000000000000,0x3ff0000000000000,EXC_CHK_NONE}, // (1 ,1.000000)^1/3 = (1 ,1.000000)
{0x3fefffffc49bd0dc,0x3fefffffec33f03d,EXC_CHK_NONE}, // (1 ,1.000000)^1/3 = (1 ,1.000000)
{0x3ff0000000000119,0x3ff000000000005e,EXC_CHK_NONE}, // (1 ,1.000000)^1/3 = (1 ,1.000000)
{0x3ff7fffef39085f4,0x3ff250bf9d679517,EXC_CHK_NONE}, // (1.5 ,1.499999)^1/3 = (1.14471 ,1.144714)
{0x3ff8000000000001,0x3ff250bfe1b082f5,EXC_CHK_NONE}, // (1.5 ,1.500000)^1/3 = (1.14471 ,1.144714)
{0xbff0000000000000,0xbff0000000000000,EXC_CHK_NONE}, // (-1 ,-1.000000)^1/3 = (-1 ,-1.000000)
{0xbfefffffc49bd0dc,0xbfefffffec33f03d,EXC_CHK_NONE}, // (-1 ,-1.000000)^1/3 = (-1 ,-1.000000)
{0xbff0000000000119,0xbff000000000005e,EXC_CHK_NONE}, // (-1 ,-1.000000)^1/3 = (-1 ,-1.000000)
{0xbff7fffef39085f4,0xbff250bf9d679517,EXC_CHK_NONE}, // (-1.5 ,-1.499999)^1/3 = (-1.14471 ,-1.144714)
{0xbff8000000000001,0xbff250bfe1b082f5,EXC_CHK_NONE}, // (-1.5 ,-1.500000)^1/3 = (-1.14471 ,-1.144714)
{0x4000000000000000,0x3ff428a2f98d728b,EXC_CHK_NONE}, // (2 ,2.000000)^1/3 = (1.25992 ,1.259921)
{0xc000000000000000,0xbff428a2f98d728b,EXC_CHK_NONE}, // (-2 ,-2.000000)^1/3 = (-1.25992 ,-1.259921)
{0x4024000000000000,0x40013c484138704f,EXC_CHK_NONE}, // (10 ,10.000000)^1/3 = (2.15443 ,2.154435)
{0xc024000000000000,0xc0013c484138704f,EXC_CHK_NONE}, // (-10 ,-10.000000)^1/3 = (-2.15443 ,-2.154435)
{0x408f400000000000,0x4024000000000000,EXC_CHK_NONE}, // (1000 ,1000.000000)^1/3 = (10 ,10.000000)
{0xc08f400000000000,0xc024000000000000,EXC_CHK_NONE}, // (-1000 ,-1000.000000)^1/3 = (-10 ,-10.000000)
{0x4050d9999999999a,0x4010474916e2d86b,EXC_CHK_NONE}, // (67.4 ,67.400000)^1/3 = (4.06961 ,4.069615)
{0xc050d9999999999a,0xc010474916e2d86b,EXC_CHK_NONE}, // (-67.4 ,-67.400000)^1/3 = (-4.06961 ,-4.069615)
{0x409efe6666666666,0x4029211d7cc31cbd,EXC_CHK_NONE}, // (1983.6 ,1983.600000)^1/3 = (12.5647 ,12.564678)
{0xc09efe6666666666,0xc029211d7cc31cbd,EXC_CHK_NONE}, // (-1983.6 ,-1983.600000)^1/3 = (-12.5647 ,-12.564678)
{0x4055e00000000000,0x4011c211887d70a0,EXC_CHK_NONE}, // (87.5 ,87.500000)^1/3 = (4.43952 ,4.439520)
{0xc055e00000000000,0xc011c211887d70a0,EXC_CHK_NONE}, // (-87.5 ,-87.500000)^1/3 = (-4.43952 ,-4.439520)
{0x41002bc800000000,0x40497d142b9b48df,EXC_CHK_NONE}, // (132473 ,132473.000000)^1/3 = (50.9772 ,50.977178)
{0xc1002bc800000000,0xc0497d142b9b48df,EXC_CHK_NONE}, // (-132473 ,-132473.000000)^1/3 = (-50.9772 ,-50.977178)
{0xc330000000000000,0xc10428a2f98d728b,EXC_CHK_NONE}, // (4.5036e+15 ,4503599627370496.000000)^1/3 = (165140 ,165140.371852)
{0x4330000000000001,0x410428a2f98d728b,EXC_CHK_NONE}, // (4.5036e+15 ,4503599627370497.000000)^1/3 = (165140 ,165140.371852)
{0x432fffffffffffff,0x410428a2f98d728b,EXC_CHK_NONE}, // (4.5036e+15 ,4503599627370495.500000)^1/3 = (165140 ,165140.371852)
{0xc330000000000000,0xc10428a2f98d728b,EXC_CHK_NONE}, // (-4.5036e+15 ,-4503599627370496.000000)^1/3 = (-165140 ,-165140.371852)
{0xc330000000000001,0xc10428a2f98d728b,EXC_CHK_NONE}, // (-4.5036e+15 ,-4503599627370497.000000)^1/3 = (-165140 ,-165140.371852)
{0xc32fffffffffffff,0xc10428a2f98d728b,EXC_CHK_NONE}, // (-4.5036e+15 ,-4503599627370495.500000)^1/3 = (-165140 ,-165140.371852)
{0x43480677adfcfa00,0x410d1589611a8eb5,EXC_CHK_NONE}, // (1.3525e+16 ,13525021061936128.000000)^1/3 = (238257 ,238257.172414)
{0x435801239871fa00,0x4112510a0da873d6,EXC_CHK_NONE}, // (2.70266e+16 ,27026607336581120.000000)^1/3 = (300099 ,300098.513338)
{0x4368111144444444,0x411718eb571e0c64,EXC_CHK_NONE}, // (5.41933e+16 ,54193322387972640.000000)^1/3 = (378427 ,378426.835076)
{0x4388ffffffffffff,0x412290fca9c761f7,EXC_CHK_NONE}, // (2.2518e+17 ,225179981368524768.000000)^1/3 = (608382 ,608382.331599)
{0x4398ffffffffffff,0x412764636974629c,EXC_CHK_NONE}, // (4.5036e+17 ,450359962737049536.000000)^1/3 = (766514 ,766513.705966)
{0x43a8ffffffffffff,0x412d78e581a0c12f,EXC_CHK_NONE}, // (9.0072e+17 ,900719925474099072.000000)^1/3 = (965747 ,965746.753180)
{0x43b8aaaa68ffffff,0x41327bc48aac21de,EXC_CHK_NONE}, // (1.77742e+18 ,1777420370899369728.000000)^1/3 = (1.21133e+06 ,1211332.541689)
{0x43c8aaaa69ffffff,0x413749a75e71ee87,EXC_CHK_NONE}, // (3.55484e+18 ,3554840750388674048.000000)^1/3 = (1.52618e+06 ,1526183.368926)
{0x43d8009678abcd00,0x413d1329cd456a1b,EXC_CHK_NONE}, // (6.91819e+18 ,6918190807731404800.000000)^1/3 = (1.90545e+06 ,1905449.801840)
{0x7fefffffffffffff,0x554428a2f98d728b,EXC_CHK_NONE}, //(1.79769e+308 ,179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000)^1/3 = (5.6438e+102 ,5643803094122362298386835960720996333881089063696129532042408160730270011663197773501163653620081623040.000000)
{0x43e8098734209870,0x4142532c0c4c744c,EXC_CHK_NONE}   , // (1.38565e+19 ,13856513517244612608.000000)^1/3 = (2.40188e+06 ,2401880.096083)
{0x43b3c4eafedcab02,0x41312b3ea8218c32,EXC_CHK_NONE}	// (1.42452e+18 ,1424521762500706816.000000)^1/3 = (1.12518e+06 ,1125182.656762)

};

