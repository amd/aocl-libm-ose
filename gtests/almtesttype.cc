/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
 */
 
#include <cstdio>
#include <string>
#include <vector>
#include "defs.h"
#include "almstruct.h"
#include "almtest.h"
#include "cmdline.h"

vector<AccuParams> accuData;
vector<SpecParams> specData;

int AlmTestFramework::AlmTestType(InputParams *params, InputData *inData, 
                                   PrintTstRes *ptr) {
  sprintf(ptr->print[ptr->tstcnt], "%s%s", "***************************",
  "*******************************************************************");
  ptr->tstcnt++;

  sprintf(ptr->print[ptr->tstcnt],"%-12s %-12s %-12s %-12s %-12s %-12s %s",
  "TEST", "TYPE", "DATATYPE", "No.Tests", "Passed", "Failed", "MAX ULP ERR" );
   ptr->tstcnt++;
    
  if (params->ttype == ALM::TestType::E_Accuracy) {
    AccuParams accup;
    memset(&accup, 0, sizeof(AccuParams));
    accup.nargs = GetnIpArgs();
    accup.inpData = inData;
    for (int i = 0; i < MAX_INPUT_RANGES; i++)
    accup.range[i] = params->range[i];
    accup.verboseflag = params->verboseflag;
    int vec_input_count = (int)params->fqty;
    if (params->fqty == ALM::FloatQuantity::E_All)
      vec_input_count = (int)ALM::FloatQuantity::E_Vector_8;

    if ((params->count % vec_input_count != 0)) {
      params->count = params->count +
                      (vec_input_count - params->count % vec_input_count);
    }
    accup.count = params->count;
    accup.prttstres = ptr;
    accuData.push_back(accup);
    return 0;    
  }
  
  if (params->ttype == ALM::TestType::E_SpecialCase) {
    SpecParams specp;
    memset(&specp, 0, sizeof(SpecParams));
    specp.nargs = GetnIpArgs();
    specp.verboseflag = params->verboseflag;
    specp.prttstres = ptr;
    switch (params->fwidth) {
      case ALM::FloatWidth::E_F32:
        SpecSetupf32(&specp);
        specData.push_back(specp);
      break;
      case ALM::FloatWidth::E_F64:
        SpecSetupf64(&specp);
        specData.push_back(specp);
      break;
      default:
        SpecSetupf32(&specp);
        SpecSetupf64(&specp);
        specData.push_back(specp);
      break;
    }
    return 0;    
  }

  if (params->ttype == ALM::TestType::E_Conformance) {
    SpecParams specp;
    memset(&specp, 0, sizeof(SpecParams));
    specp.nargs = GetnIpArgs();
    specp.verboseflag = params->verboseflag;
    specp.prttstres = ptr;
    switch (params->fwidth) {
      case ALM::FloatWidth::E_F32:
        ConfSetupf32(&specp);
        specData.push_back(specp);
      break;
      case ALM::FloatWidth::E_F64:
        ConfSetupf64(&specp);
        specData.push_back(specp);
      break;
      default:
        ConfSetupf32(&specp);
        ConfSetupf64(&specp);
        specData.push_back(specp);
      break;
    }
    return 0;    
  }
  return 0;  
}

AlmTestFramework::~AlmTestFramework() {
#if defined(DEBUG_PRINTS) || defined(TRACE_LOG)
  cout << "AlmTestFramework destructor completed" << endl;
#endif
}
