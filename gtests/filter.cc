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
#include "almtest.h"
#include "cmdline.h"

void StringAppend(string &str, string sfwidth, FloatQuantity fqty) {
  if (fqty != ALM::FloatQuantity::E_Scalar) {
    sfwidth = sfwidth + "S";
  }
  str = str + sfwidth + "*:";
}

/*
 * This method will check if the current function
 * under execution is a complex number variant or not.
 */
bool isComplexFunction(string s)
{
    if( (s == "cexp") ||
        (s == "clog") ||
        (s == "cpow") )
    {
        return true;
    }
    return false;
}

void SubFilterFwidth(InputParams *params, string &filter_data,
                                                 FloatQuantity fqty) {
  string str("");
  string sfwidth("");
  if(isComplexFunction(params->testFunction))
  {
    filter_data = filter_data + "COMPLEX_";
  }
  switch (params->fwidth) {
    case ALM::FloatWidth::E_F64:
      sfwidth = filter_data + "DOUBLE";
      StringAppend(str, sfwidth, fqty);
    break;
    case ALM::FloatWidth::E_F32:
      sfwidth = filter_data + "FLOAT";
      StringAppend(str, sfwidth, fqty);
    break;
    default:
      sfwidth = filter_data + "FLOAT";
      StringAppend(str, sfwidth, fqty);
      sfwidth = filter_data + "DOUBLE";
      StringAppend(str, sfwidth, fqty);
    break;
  }
  filter_data = str;
}

void SubFilterFqty(InputParams *params, string &filter_data) {
  string str("");

  switch (params->fqty) {
    case ALM::FloatQuantity::E_Vector_Array:
      str = filter_data + "_VECTOR_ARRAY_";
      SubFilterFwidth(params, str, ALM::FloatQuantity::E_Vector_Array);
    break;
    case ALM::FloatQuantity::E_Scalar:
      str = filter_data + "_SCALAR_";
      SubFilterFwidth(params, str, ALM::FloatQuantity::E_Scalar);
    break;

    case ALM::FloatQuantity::E_Vector_2:
      str = filter_data + "_VECTOR_2";
      SubFilterFwidth(params, str, ALM::FloatQuantity::E_Vector_2);
    break;

    case ALM::FloatQuantity::E_Vector_4:
      str = filter_data + "_VECTOR_4";
      SubFilterFwidth(params, str, ALM::FloatQuantity::E_Vector_4);
    break;

    case ALM::FloatQuantity::E_Vector_8:
      str = filter_data + "_VECTOR_8";
      SubFilterFwidth(params, str, ALM::FloatQuantity::E_Vector_8);
    break;

    case ALM::FloatQuantity::E_Vector_16:
      str = filter_data + "_VECTOR_16";
      SubFilterFwidth(params, str, ALM::FloatQuantity::E_Vector_16);
    break;

    default:
      string sfqty("");

      sfqty = filter_data + "_VECTOR_ARRAY_";
      SubFilterFwidth(params, sfqty, ALM::FloatQuantity::E_Scalar);
      str = str + sfqty;

      sfqty = filter_data + "_SCALAR_";
      SubFilterFwidth(params, sfqty, ALM::FloatQuantity::E_Scalar);
      str = str + sfqty;

      sfqty = filter_data + "_VECTOR_2";
      SubFilterFwidth(params, sfqty, ALM::FloatQuantity::E_Vector_2);
      str = str + sfqty;

      sfqty = filter_data + "_VECTOR_4";
      SubFilterFwidth(params, sfqty, ALM::FloatQuantity::E_Vector_4);
      str = str + sfqty;

      sfqty = filter_data + "_VECTOR_8";
      SubFilterFwidth(params, sfqty, ALM::FloatQuantity::E_Vector_8);
      str = str + sfqty;

      sfqty = filter_data + "_VECTOR_16";
      SubFilterFwidth(params, sfqty, ALM::FloatQuantity::E_Vector_16);
      str = str + sfqty;

    break;
  }
  filter_data = str;
}

void AlmTestFramework::CreateGtestFilters(InputParams *params,
                                          string &ttype) {
  switch (params->ttype) {
    case ALM::TestType::E_Conformance:
    {
      FloatQuantity fqty = ALM::FloatQuantity::E_Scalar;
      ttype = ttype + "*CONFORMANCE_";
      SubFilterFwidth(params, ttype, fqty);
    }
    break;
    case ALM::TestType::E_SpecialCase:
    {
      FloatQuantity fqty = ALM::FloatQuantity::E_Scalar;
      ttype = ttype + "*SPECIALCASE_";
      SubFilterFwidth(params, ttype, fqty);
    }
    break;
    case ALM::TestType::E_Accuracy:
    default:
    {
      ttype = ttype + "*ACCURACY";
      SubFilterFqty(params, ttype);

      /* vector 2 elem float and 16 elem double are invalid cases for now */
      string toErase ="*ACCURACY_VECTOR_16DOUBLES*:";
      size_t pos;
      while ((pos  = ttype.find(toErase))!= std::string::npos) {
          ttype.erase(pos, toErase.length());
      }
      toErase ="*ACCURACY_VECTOR_2FLOATS*:";
      while ((pos  = ttype.find(toErase) )!= std::string::npos) {
          ttype.erase(pos, toErase.length());
      }
    }
    break;
  }
}
