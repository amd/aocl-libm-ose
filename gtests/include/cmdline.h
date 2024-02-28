/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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


#ifndef __CMDLINE_H__
#define __CMDLINE_H__

#pragma once
#include "args.h"
#include "defs.h"
#include "almtest.h"
#include "almtestperf.h"

namespace ALM {

struct RangeReader {
  void operator()(const std::string &name, const std::string &value,
                  ALM::Range &dest);
};

struct ToLowerReader {
  bool operator()(const std::string &name, const std::string &value,
                  std::string &dest);
};

namespace Test {

void BenchCommandParser(args::Subparser &parser);
void TestCommandParser(args::Subparser &parser);

class cmdLine {
  args::ArgumentParser *parser;

  args::Group *commands;
  args::Group *arguments;

  args::Command  *bench;
  args::Command  *test;

  args::GlobalOptions *globals;

  args::ValueFlag<uint64_t> *count;
  args::ValueFlag<uint64_t> *loop;
  args::ValueFlag<uint64_t> *Iterations;
  args::ValueFlag<std::string> *inputdesc;
  args::ValueFlag<std::string> *inputfile;

  args::MapFlag<std::string, RoundingMode, ToLowerReader> *rounding;

  args::MapFlag<std::string, TestType, ToLowerReader> *testtype;

  args::ValueFlag<uint32_t> *nvector;
  args::ValueFlag<uint32_t> *verbose;
  args::HelpFlag *help;

  args::ValueFlagList<Range, RangeReader> *ranges;

 public:
  cmdLine();
  ~cmdLine() {}

  bool Parse(int argc, char *argv[]);
  bool Echo(InputParams *);
  bool Validate();
  friend void ALM::Test::BenchCommandParser(args::Subparser &parser);
  friend void ALM::Test::TestCommandParser(args::Subparser &parser);
};

cmdLine *cmdLineStart(void);
} // namespace Test
} // namespace ALM
#endif
