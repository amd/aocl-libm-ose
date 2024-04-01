/*
 * Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
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

    accup.vec_input_count = (int)params->fqty;
    if (params->fqty == ALM::FloatQuantity::E_All)
      accup.vec_input_count = (int)ALM::FloatQuantity::E_Vector_16;

    if (params->fqty != ALM::FloatQuantity::E_Vector_Array)
    {
      if ((params->count % accup.vec_input_count != 0)) {
        params->count = params->count +
                        (accup.vec_input_count - params->count % accup.vec_input_count);
      }
    }

    accup.count = params->count;
    accup.prttstres = ptr;
    accuData.push_back(accup);
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
