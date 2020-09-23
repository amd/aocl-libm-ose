#ifndef __CMDLINE_H__
#define __CMDLINE_H__

/*
 * Copyright (C) 2019-2020, AMD. All rights are reserved
 *
 * Author: Prem Mallappa<pmallapp@amd.com>
 */

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
