/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
 */
 
#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include "args.h"
#include "defs.h"
#include "cmdline.h"
#include "almstruct.h"

uint32_t dbg_bits = DBG_DEFAULT;

namespace ALM {

void RangeReader::operator()(const std::string &name, const std::string &value,
                             ALM::Range &dest) {
  size_t delimpos = 0;
  size_t pos = 0;

  // parse the 'start'
  dest.r_range.first = std::stod(value, &delimpos);

  // Parse the 'end'
  if (delimpos != value.length())
    dest.r_range.second = std::stod(&value[delimpos + 1], &pos);

  delimpos = delimpos + pos + 1;

  // Parse the distribution type
  if (delimpos != value.length()) {
    auto dist = std::string(value, (delimpos+1));
    if (dist == "linear")
      dest.r_type = RangeType::E_Linear;
    else if (dist == "random")
      dest.r_type = RangeType::E_Random;
    else
      dest.r_type = RangeType::E_Simple;
  }
  else
    dest.r_type = RangeType::E_Simple;
}

bool ToLowerReader::operator()(const std::string &name,
                               const std::string &value,
                               std::string &destination) {
  destination = value;
  std::transform(destination.begin(), destination.end(), destination.begin(),
                 ::tolower);
  return true;
}

namespace Test {
static cmdLine cmd;

cmdLine *cmdLineStart(void) { return &cmd; }

cmdLine::cmdLine() : arguments(NULL) {
  parser = new args::ArgumentParser("Testing tool for AMD LibM",
                                    "Author: Prem Mallappa <pmallapp@amd.com>");

  cmd.arguments =
      new args::Group(*parser, "arguments", args::Group::Validators::DontCare,
                      args::Options::Global);

  std::unordered_map<std::string, TestType> test_map{
      {"accu", TestType::E_Accuracy},
      {"spec", TestType::E_SpecialCase},
      {"conf", TestType::E_Conformance},
      {"perf", TestType::E_Performance},
  };
  
  cmd.testtype = new args::MapFlag<std::string, TestType, ToLowerReader>(
      *cmd.parser, "type", 
      "TestType <type> = [accu, spec, conf, perf]",
      {'t', "type"}, test_map);
      
  cmd.Iterations = new args::ValueFlag<uint64_t>(
      *cmd.arguments, "Iterations", "No. of Iterations", {'n',"Iterations"});

  cmd.count = new args::ValueFlag<uint64_t>(
      *cmd.arguments, "count", "No of input to process", {'c',"count"});

  cmd.inputdesc =
      new args::ValueFlag<std::string>(*cmd.arguments, "input",
                                       "Input (Datatype) Description \n"
                                       "prefix: \n"
                                       "half->h, float ->f \n"
                                       "double->d quad->q \n"
                                       "long double->ld \n"
                                       "suffix: \n"
                                       "Complex : c  \t Others    : <none>",
                                       {'i', "input"});

  cmd.nvector = new args::ValueFlag<uint32_t>(
      *cmd.arguments, "n-vector", "No. of elements in vector", {'e', "vector"});

  cmd.inputfile = new args::ValueFlag<std::string>(
      *cmd.arguments, "file", "Filename to read Inputs from",
      {'f', "input-file"});

  cmd.ranges = new args::ValueFlagList<Range, ALM::RangeReader>[MAX_INPUT_RANGES]{{
      *cmd.arguments, "ranges", "Input Ranges", {'r', "range"}},
   	  {*cmd.arguments, "ranges", "Input Ranges", {'r', "range"}}};

  std::unordered_map<std::string, RoundingMode> map{
      {"neareven", RoundingMode::E_NearestEven},
      {"nearaway", RoundingMode::E_NearestAway},
      {"tozero", RoundingMode::E_Zero},
      {"toneginf", RoundingMode::E_NegativeInf},
      {"toinf", RoundingMode::E_PositiveInf},
  };
  cmd.rounding = new args::MapFlag<std::string, RoundingMode, ToLowerReader>(
      *cmd.parser, "rounding",
      "Rounding mode [neareven, nearaway, tozero, toneginf, toinf]",
      {"rounding"}, map);

  cmd.verbose = new args::ValueFlag<uint32_t>(*cmd.arguments, "verbose",
                                      "Increase verbosity", {'v', "verbose"});

  help =
      new args::HelpFlag(*parser, "help", "Display this help", {'h', "help"});
}

bool cmdLine::Parse(int argc, char *argv[]) {
  try {
    if(argc == 1) {
      std::cout << *parser;     
      return 1;
    } else { 
      parser->ParseCLI(argc, argv);
    }
  } catch (const args::Completion &e) {
    std::cout << e.what() << std::endl;
    return 0;
  } catch (const args::Help &) {
    std::cout << *parser;
    return 0;
  } catch (const args::ParseError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << *parser;
    return 1;
  }

  return 0;
}

bool cmdLine::Echo(InputParams *inparams) {
  if (testtype && *testtype) {
    std::cout << "Test Type       : " << args::get(*testtype) << std::endl;
    inparams->ttype = (enum TestType)args::get(*testtype);
  } else {
    return 1;
  }

  if (*inputfile) {
    std::cout << "File: " << args::get(*inputfile) << std::endl;
  }

  if (*ranges) {
	int i = 0;
    for (auto &&r : args::get(*ranges)) {
      std::cout << "Range           : [" << r.r_range.first << ", "
                << r.r_range.second << "] " << r.r_type << endl;
      inparams->range[i].min = r.r_range.first;
      inparams->range[i].max = r.r_range.second;
      inparams->range[i].type = (enum RangeType)r.r_type;
      i++;
    }
  } else {
    for(auto i = 0; i < MAX_INPUT_RANGES; i++) {
    inparams->range[i].min = DBL_MIN;
    inparams->range[i].max = DBL_MAX;
    inparams->range[i].type = (enum RangeType)0;
    }
  }

  if (*verbose) {
    std::cout << "Verbosity       : " << args::get(*verbose) << std::endl;
    inparams->verboseflag = args::get(*verbose);
    int32_t bit = inparams->verboseflag;
    int high = LIBM_TEST_DBG_VERBOSE3 - LIBM_TEST_DBG_INFO;
    
    /* adjust between 0-5 */
    bit = (bit < 0)? 0: bit;
    bit = (bit > high)? high: bit;
    dbg_bits = (1 << (bit + LIBM_TEST_DBG_INFO + 1)) - 1;
  }

  if((inparams->ttype != ALM::TestType::E_Accuracy) ||
    (inparams->ttype != ALM::TestType::E_SpecialCase) || 
    (inparams->ttype != ALM::TestType::E_Conformance)) {
      if (*Iterations) {
        std::cout << "No. of Iterations  : " << args::get(*Iterations) << std::endl;
        inparams->niter = args::get(*Iterations);
      } else {
        inparams->niter = NITER;
      }
  }
    
  if (*count) {
    std::cout << "No. of Samples  : " << args::get(*count) << std::endl;
    inparams->count = args::get(*count);
  } else {
    inparams->count = NELEM;
  }

  if (*inputdesc) {
    auto datatype = args::get(*inputdesc);
    if ((strcmp(datatype.c_str(), "float") == 0) ||
        (strcmp(datatype.c_str(), "f") == 0)) {
      inparams->fwidth = ALM::FloatWidth::E_F32;
    } else if ((strcmp(datatype.c_str(), "double") == 0) ||
               (strcmp(datatype.c_str(), "d") == 0)) {
      inparams->fwidth = ALM::FloatWidth::E_F64;
    } else {
      cout << "Invalid Data_Type Option" << endl;
    }
  } else {
    inparams->fwidth = ALM::FloatWidth::E_ALL;
  }
  std::cout << "Data_Type       : " << inparams->fwidth << std::endl;

  if (*nvector) {
    auto n = args::get(*nvector);
    switch (n) {
      case 1:
        inparams->fqty = ALM::FloatQuantity::E_Scalar;
        break;
      case 2:
        inparams->fqty = ALM::FloatQuantity::E_Vector_2;
        break;
      case 4:
        inparams->fqty = ALM::FloatQuantity::E_Vector_4;
        break;
      case 8:
        inparams->fqty = ALM::FloatQuantity::E_Vector_8;
        break;
      default:
        break;
    }
  } else {
    inparams->fqty = ALM::FloatQuantity::E_All;
  }
  std::cout << "Elements per Lane : " << inparams->fqty << std::endl;
    
  if (((inparams->fwidth == ALM::FloatWidth::E_F32) &&
     (inparams->fqty == ALM::FloatQuantity::E_Vector_2)) ||
     ((inparams->fwidth == ALM::FloatWidth::E_F64) &&
     (inparams->fqty == ALM::FloatQuantity::E_Vector_8))) {
      cout << "Invalid Options" << endl;
    return 1;
  }

  return 0;
}

bool cmdLine::Validate() { return true; }
}  // namespace Test
}  // namespace ALM
