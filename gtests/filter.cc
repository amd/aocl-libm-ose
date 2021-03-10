/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
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

void SubFilterFwidth(InputParams *params, string &filter_data,
                                                 FloatQuantity fqty) {
  string str("");
  string sfwidth("");
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
    default:
      string sfqty("");

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

      string toErase ="*ACCURACY_VECTOR_8DOUBLES*:";
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
  cout << "Filter_data :" << ttype.c_str() << endl;
}
