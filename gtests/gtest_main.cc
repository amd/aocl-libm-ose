/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
 */
 
#include <cstdio>
#include <iostream>
#include <string>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <vector>
#include "almstruct.h"
#include "gtest.h"
#include "defs.h"
#include "almtest.h"
#include "cmdline.h"
#include "callback.h"

extern vector<AccuParams> accuData;
extern vector<SpecParams> specData;

TEST_P(AccuTestFixtureFloat, ACCURACY_SCALAR_FLOAT) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[2];
  
  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;
   
  for (uint32_t i = 0; i < count; i++) {
    test_s1s(&data, i);

    ip[0] = inpbuff[i];
    if(nargs == 2)    
      ip[1] = inpbuff1[i];
    
    float exptd = getExpected(ip);    
    double ulp = getUlp(aop[0], exptd);
    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
      nfail++;
    }

    if (vflag == 1) {
      EXPECT_LT(ulp, inData->ulp_threshold)
            << "Input:[" << ip[0] << " " << ip[1] <<"]Actual:" << aop[0]
          << " Expected:" << exptd << " ULP: " << ulp << endl;
    } else {
      EXPECT_LT(ulp, inData->ulp_threshold);
      LIBM_TEST_DPRINTF(VERBOSE2, ,
                      "Input:[", ip[0], " ", ip[1], "]Actual:", aop[0], 
                      " Expected:", exptd, " ULP: ",ulp);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Scalar","Accuracy","s1s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;  
}

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_4FLOATS) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[2];
  
  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;
  
  for (uint32_t i = 0; i < count; i += 4) {
    test_v4s(&data, i);

    for (uint32_t j = 0; j < 4; j++) {
      ip[0] = inpbuff[i + j];
      if(nargs == 2)    
        ip[1] = inpbuff1[i + j];
      
      float exptd = getExpected(ip);
      double ulp = getUlp(aop[j], exptd);
      if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
        nfail++;
      }
      
      if (vflag == 1) {
        EXPECT_LT(ulp, inData->ulp_threshold)
            << "Input:[" << ip[0] << " " << ip[1] <<"]Actual:" << aop[j]
            << " Expected:" << exptd << " ULP: " << ulp << endl;
      } else {
        EXPECT_LT(ulp, inData->ulp_threshold);
        LIBM_TEST_DPRINTF(VERBOSE2, ,
                      "Input:[", ip[0], " ", ip[1], "]Actual:", aop[j], 
                      " Expected:", exptd, " ULP: ",ulp);
      }
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v4s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;  
}

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_8FLOATS) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[2];
  
  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;
  
  for (uint32_t i = 0; i < count; i += 8) {
    test_v8s(&data, i);

    for (uint32_t j = 0; j < 8; j++) {
      ip[0] = inpbuff[i + j];
      if(nargs == 2)    
        ip[1] = inpbuff1[i + j];
       
      float exptd = getExpected(ip);      
      double ulp = getUlp(aop[j], exptd);
      if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
        nfail++;
      }

      if (vflag == 1) {
        EXPECT_LT(ulp, inData->ulp_threshold)
            << "Input:[" << ip[0] << " " << ip[1] <<"]Actual:" << aop[j]
            << " Expected:" << exptd << " ULP: " << ulp << endl;
      } else { 
        EXPECT_LT(ulp, inData->ulp_threshold);
        LIBM_TEST_DPRINTF(VERBOSE2, ,
                      "Input:[", ip[0], " ", ip[1], "]Actual:", aop[j], 
                      " Expected:", exptd, " ULP: ",ulp);
      }
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v8s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;  
}

TEST_P(AccuTestFixtureDouble, ACCURACY_SCALAR_DOUBLE) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  double ip[2];
  
  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;
  
  for (uint32_t i = 0; i < count; i++) {
    test_s1d(&data, i);

    ip[0] = inpbuff[i];
    if(nargs == 2)    
      ip[1] = inpbuff1[i];

    double exptd = getExpected(ip);
    double ulp = getUlp(aop[0], exptd);
    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
      nfail++;
    }

    if (vflag == 1) {
      EXPECT_LT(ulp, inData->ulp_threshold)
            << "Input:[" << ip[0] << " " << ip[1] <<"]Actual:" << aop[0]
          << " Expected:" << exptd << " ULP: " << ulp << endl;
    } else { 
      EXPECT_LT(ulp, inData->ulp_threshold);
      LIBM_TEST_DPRINTF(VERBOSE2, ,
                      "Input:[", ip[0], " ", ip[1], "]Actual:", aop[0], 
                      " Expected:", exptd, " ULP: ",ulp);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Scalar","Accuracy","s1d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;  
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_2DOUBLES) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  double ip[2];
  
  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;
  
  for (uint32_t i = 0; i < count; i += 2) {
    test_v2d(&data, i);

    for (uint32_t j = 0; j < 2; j++) {
      ip[0] = inpbuff[i + j];
      if(nargs == 2)    
        ip[1] = inpbuff1[i + j];

      double exptd = getExpected(ip);
      double ulp = getUlp(aop[j], exptd);
      if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
        nfail++;
      }

      if (vflag == 1) {
        EXPECT_LT(ulp, inData->ulp_threshold)
            << "Input:[" << ip[0] << " " << ip[1] <<"]Actual:" << aop[j]
            << " Expected:" << exptd << " ULP: " << ulp << endl;
      } else { 
        EXPECT_LT(ulp, inData->ulp_threshold);
        LIBM_TEST_DPRINTF(VERBOSE2, ,
                      "Input:[", ip[0], " ", ip[1], "]Actual:", aop[j], 
                      " Expected:", exptd, " ULP: ",ulp);
      }
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v2d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;  
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_4DOUBLES) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  double ip[2];
  
  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;
  
  for (uint32_t i = 0; i < count; i += 4) {
    test_v4d(&data, i);

    for (uint32_t j = 0; j < 4; j++) {
      ip[0] = inpbuff[i + j];
      if(nargs == 2)    
        ip[1] = inpbuff1[i + j];

      double exptd = getExpected(ip);
      double ulp = getUlp(aop[j], exptd);
      if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
        nfail++;
      }

      if (vflag == 1) {
        EXPECT_LT(ulp, inData->ulp_threshold)
            << "Input:[" << ip[0] << " " << ip[1] <<"]Actual:" << aop[j]
            << " Expected:" << exptd << " ULP: " << ulp << endl;
      } else { 
        EXPECT_LT(ulp, inData->ulp_threshold);
        LIBM_TEST_DPRINTF(VERBOSE2, ,
                      "Input:[", ip[0], " ", ip[1], "]Actual:", aop[j], 
                      " Expected:", exptd, " ULP: ",ulp);
      }
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v4d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;  
}

TEST_P(SpecTestFixtureFloat, CONFORMANCE_FLOAT) {
  int nfail = 0; 
  float aop, op;
  float ip[2];
  test_data t;
  t.ip  = (void *)data;
  t.op  = (void *)&aop;

  if(nargs == 2)
    t.ip1 = (void *)data1;

  for (uint32_t i = 0; i < count; i++) {
    test_s1s(&t, i);

    int eef = expected_expection[i];
    
    ip[0] = data[i];
    if(nargs == 2)  {
      ip[1] = data1[i];
    }
    op = getExpected(ip);
    
    if (vflag == 1) {
      EXPECT_PRED3(SpecTestFixtureFloat::ConfTestVerifyFloat, &ip[0], eef, &nfail)
          << "Input:[" << ip[0] << " " << ip[1] <<"]"
          << "Actual " << aop << " Expected " << op << endl;
    } else { 
      EXPECT_PRED3(SpecTestFixtureFloat::ConfTestVerifyFloat, &ip[0], eef, &nfail);
      LIBM_TEST_DPRINTF(VERBOSE2, ,
                      "Input:[", ip[0], " ", ip[1], "]",      
                      " Actual:", aop, " Expected:", op);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Conformance","s1s",count,(count-nfail), nfail);
  ptr->tstcnt++;  
}

TEST_P(SpecTestFixtureDouble, CONFORMANCE_DOUBLE) {
  int nfail = 0;
  double aop, op;
  double ip[2];
  test_data t;
  t.ip  = (void *)data;
  t.op  = (void *)&aop;

  if(nargs == 2)
    t.ip1 = (void *)data1;

  for (uint32_t i = 0; i < count; i++) {
    test_s1d(&t, i);
    
    int eef = expected_expection[i];    
    ip[0] = data[i];

    if(nargs == 2)  {
      ip[1] = data1[i];
    }
    op = getExpected(ip);     

    if (vflag == 1) {
      EXPECT_PRED3(SpecTestFixtureDouble::ConfTestVerifyDouble, &ip[0], eef, &nfail)
          << "Input:[" << ip[0] << " " << ip[1] <<"]"      
          << "Actual " << aop << " Expected " << op << endl;
    } else { 
      EXPECT_PRED3(SpecTestFixtureDouble::ConfTestVerifyDouble, &ip[0], eef, &nfail);
      LIBM_TEST_DPRINTF(VERBOSE2, ,
                      "Input:[", ip[0], " ", ip[1], "]",      
                      " Actual:", aop, " Expected:", op);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Conformance","s1d",count,(count-nfail), nfail);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureFloat, SPECIALCASE_FLOAT) {
  int nfail = 0; 
  float aop, op;
  float ip[2];
  test_data t;
  t.ip  = (void *)data;
  t.op  = (void *)&aop;

  if(nargs == 2)
    t.ip1 = (void *)data1;

  for (uint32_t i = 0; i < count; i++) {
    test_s1s(&t, i);

    ip[0] = data[i];
    if(nargs == 2)  {
      ip[1] = data1[i];
    }     
    op = getExpected(ip);
    
    if (vflag == 1) {
      EXPECT_PRED3(SpecTestFixtureFloat::SpecialVerifyFloat, aop, op, &nfail)
          << "Input:[" << ip[0] << " " << ip[1] <<"]"      
          << "Actual " << aop << " Expected " << op << endl;
    } else { 
      EXPECT_PRED3(SpecTestFixtureFloat::SpecialVerifyFloat, aop, op, &nfail);
      LIBM_TEST_DPRINTF(VERBOSE2, ,
                      "Input:[", ip[0], " ", ip[1], "]",
                      " Actual:", aop, " Expected:", op);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Special","s1s",count,(count-nfail), nfail);
  ptr->tstcnt++;  
}

TEST_P(SpecTestFixtureDouble, SPECIALCASE_DOUBLE) {
  int nfail = 0; 
  double aop, op;
  double ip[2];
  test_data t;
  t.ip  = (void *)data;
  t.op  = (void *)&aop;

  if(nargs == 2)
    t.ip1 = (void *)data1;
   
  for (uint32_t i = 0; i < count; i++) {
    test_s1d(&t, i);

    ip[0] = data[i];
    if(nargs == 2)  {
      ip[1] = data1[i];
    }
    op = getGlibcOp(ip);     

    if (vflag == 1) {
      EXPECT_PRED3(SpecTestFixtureDouble::SpecialVerifyDouble, aop, op, &nfail)
          << "Input:[" << ip[0] << " " << ip[1] <<"]"      
          << "Actual " << aop << " Expected " << op << endl;
    } else { 
      EXPECT_PRED3(SpecTestFixtureDouble::SpecialVerifyDouble, aop, op, &nfail);
      LIBM_TEST_DPRINTF(VERBOSE2, ,
                      "Input:[", ip[0], " ", ip[1], "]",      
                      " Actual:", aop, " Expected:", op);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Special","s1d",count,(count-nfail), nfail);
  ptr->tstcnt++;  
}
/*****************************************************************************/
/***                            INSTANTIATE_TEST_SUITE_P                   ***/
/*****************************************************************************/
INSTANTIATE_TEST_SUITE_P(AccuTests, AccuTestFixtureFloat,
                         ::testing::ValuesIn(accuData));

INSTANTIATE_TEST_SUITE_P(AccuTests, AccuTestFixtureDouble,
                         ::testing::ValuesIn(accuData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureFloat,
                         ::testing::ValuesIn(specData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureDouble,
                         ::testing::ValuesIn(specData));
/*****************************************************************************/

int gtest_main(int argc, char **argv, InputParams *inparams) {
  AlmTestFramework almTest;
  string filter_data("");
  InputData *inData = NULL;
  PrintTstRes *ptr = NULL;
  
  inData = almTest.getInputData();
  ptr    = almTest.getPrintTetRes();
  
  memset(inData, 0, sizeof(InputData));
  inData->max_ulp_err = 0.0;
  inData->ulp_threshold = 0.5;

  memset(ptr, 0, sizeof(PrintTstRes));
  almTest.AlmTestType(inparams, inData, ptr);
  almTest.CreateGtestFilters(inparams, filter_data);

  ::testing::GTEST_FLAG(filter) = filter_data.c_str();
  testing::InitGoogleTest(&argc, argv);
  int ret_val = RUN_ALL_TESTS();
  
  cout << ptr->print[0] << endl;
  cout << ptr->print[1] << endl;  
  cout << ptr->print[0] << endl;
  for (uint64_t i = 2; i < ptr->tstcnt ; i++ ) {
    if(strlen(ptr->print[i]) != 0) {
      cout << ptr->print[i] << endl;
    }
  }
  cout << ptr->print[0] << endl;  
  return ret_val;
}

