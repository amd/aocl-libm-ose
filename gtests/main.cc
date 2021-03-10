/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
 */
 
#include <cstdio>
#include <iostream>
#include <string>
#include "almstruct.h"
#include "cmdline.h"


int main(int argc, char **argv) {
  InputParams params;
 
  ALM::Test::cmdLine *cmd = ALM::Test::cmdLineStart();

  if (cmd->Parse(argc, argv))
    return EXIT_FAILURE;

  cmd->Validate();

  memset((void *)&params, 0, sizeof(InputParams));
  if (cmd->Echo(&params))
    return EXIT_FAILURE;
  
  if((params.ttype == ALM::TestType::E_Accuracy) ||
    (params.ttype == ALM::TestType::E_SpecialCase) || 
    (params.ttype == ALM::TestType::E_Conformance)) {
      gtest_main(argc, argv, &params);
    } else { 
      gbench_main(argc, argv, &params);
    } 

  return 0;
}