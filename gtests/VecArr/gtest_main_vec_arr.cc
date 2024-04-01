/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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

#include "gtest.h"
#include "almtest.h"
#include "callback.h"

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_ARRAY_FLOATS) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[2];

  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;

  test_vas(&data, count);
  for (uint32_t i = 0; i < count; i++)
  {
    ip[0] = inpbuff[i];
    if(nargs == 2)
      ip[1] = inpbuff1[i];

    double exptd = getExpected(ip);
    double ulp = getUlp(aop[i], exptd);
    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold))
    {
      nfail++;
    }

    if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd, aop[i], ulp);
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vec_Array","Accuracy","vas",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_ARRAY_DOUBLES) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  double ip[2];

  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;

  test_vad(&data, count);
  for (uint32_t i = 0; i < count; i++)
  {
    ip[0] = inpbuff[i];
    if(nargs == 2)
      ip[1] = inpbuff1[i];

    long double exptd = getExpected(ip);
    double ulp = getUlp(aop[i], exptd);
    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold))
    {
      nfail++;
    }

    if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsDouble(nargs, ip[0], ip[1], exptd, aop[i], ulp);
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vec_Array","Accuracy","vad",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}
