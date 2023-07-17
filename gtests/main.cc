/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
#include <iostream>
#include <string>
#include "almstruct.h"
#include "cmdline.h"

/*
 * This method will trim the first argument (argv[0])
 * and return the function name which is currently being tested.
 */
string getFunctionName(string s)
{
  size_t pos = s.find("test_");
  s.erase(0, pos+5);
  return s;
}

int main(int argc, char **argv) {
  InputParams params;
 
  ALM::Test::cmdLine *cmd = ALM::Test::cmdLineStart();

  if (cmd->Parse(argc, argv))
    return EXIT_FAILURE;

  cmd->Validate();

  memset((void *)&params, 0, sizeof(InputParams));
  params.testFunction = getFunctionName(argv[0]);
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