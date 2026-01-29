/*
 * Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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

#pragma once

#include <iostream>
#include "alm_test.h"
#include <string>
#include <vector>
/*
 * InputRange:
 * Represents a numeric input range for range-based tests.
 */
struct InputRange {
    std::string srt;     /* Start value */
    std::string stp;     /* Stop value */
    std::string type;    /* Range generation type (e.g., linear, expstep) */
    std::string count;   /* Number of values to generate */
};

/*
 * YamlInputs:
 * Holds test configuration data parsed from YAML.
 */
struct YamlInputs {
    std::string test_type;               /* Type of test: unit_test, range_test, etc. */
    std::string api_name;                /* Name of the API being tested */
    std::string test_id;                 /* Unique test identifier */
    std::string variants;                /* Semicolon-separated list of variants */
    std::vector<std::string> input;      /* Single input values */
    std::string xv;                      /* Expected output value */
    std::string xxv;                     /* Expected floating-point exception */
    std::string ulp_threshold;           /* ULP threshold for accuracy tests */
    std::vector<InputRange> range;       /* Input ranges for range tests */
};

/*
 * YamlOutputs:
 * Structure to hold test outputs for YAML serialization.
 */
template <typename S>
struct YamlOutputs {
    std::string variant;                     /* Variant name (e.g., sd, vrs4, vrda) */
    std::string api_name;                    /* API name */
    std::string test_id;                     /* Unique test identifier */
    uint64_t    n[MAX_IPPTR];                 /* Number of elements in each input arguments*/
    S          *iptr[MAX_IPPTR];             /* Input pointers */
    S          *optr[MAX_OPPTR];             /* Output pointers */
    double     *ulp;                         /* ULP error values */
    int        *status;                     /* Status flags for each input */
    bool        utflag;                      /* Unit test flag */
    int         exception_raised;            /* Raised exceptions */
    double      ulp_threshold;               /* ULP threshold for accuracy tests */
    double      duration;                    /* Execution duration */
    bool        is_vra;                      /* Vectorized real array flag */
    TestMode    test_mode;                   /* Test mode (accuracy or performance) */
    std::string vendor;                      /* Vendor name */
    std::string outfile;                     /* Output yaml file name */


    /* Constructor */
    YamlOutputs(std::string &var)
        : variant(var), ulp(nullptr), status(nullptr),
          utflag(false), exception_raised(0),
          duration(0.0), is_vra(false), test_mode(TestMode::E_ACCURACY),
          vendor("amd"), outfile("amd_api_ut_ss.yaml")
    {
        std::fill(std::begin(n), std::end(n), 0);
        std::fill(std::begin(iptr), std::end(iptr), nullptr);
        std::fill(std::begin(optr), std::end(optr), nullptr);
    }
};

/*
 * TestMetadata:
 * Structure to hold comprehensive test metadata information.
 */
template <typename S>
struct TestMetadata {
    // System and build information
    std::string test_suite_name;             /* Name of the test suite */
    std::string test_version;                /* Version of the test suite */
    std::string build_date;                  /* Build timestamp */

    // Library information
    std::string library_name;                /* Full library name */
    std::string library_version;             /* Library version */
    std::string library;                     /* Short library identifier */

    // Test execution information
    std::string test_runner;                 /* Test runner name */

    // Test-specific information
    std::string api_name;                    /* API being tested */
    std::string test_id;                     /* Unique test identifier */
    std::string description;                 /* Test description */
    std::string test_type;                   /* Type of test */
    std::string variant;                     /* Test variant */
    std::string input_range;                 /* Input range description */
    uint64_t    count;                       /* Number of test cases */
    double      threshold;                   /* Error threshold */

    /* Constructor */
    TestMetadata()
        : test_suite_name("AMD Math Library Test Suite"),
          test_version("1.0"),
          build_date("unknown"),

          library_name("AMD Optimized CPU Libraries - LibM"),
          library_version("unknown"),
          library("aocl-libm"),

          test_runner("libm_testsuite"),

          api_name("unknown"),
          test_id("unknown"),
          description("unknown"),
          test_type("unknown"),
          variant("unknown"),
          input_range("unknown"),
          count(0),
          threshold(0.0) {}
};

// Configure optional filters from main.cc
void set_api_filter(const std::string& api);
void set_type_filter(const std::string& type);

int read_yaml_file(const std::string &filename, std::vector<struct YamlInputs> &params);

template <typename S>
void write_yaml_output(const struct YamlOutputs<S> *yop);

template <typename S>
void write_test_metadata(const struct TestMetadata<S> *metadata, const std::string &filename);

template <typename S>
void populate_system_metadata(struct TestMetadata<S> *metadata);

template <typename S>
struct TestMetadata<S>* get_metadata();

template <typename S>
void del_metadata(struct TestMetadata<S>* metadata);


// Structure to hold and print metrics
struct PrintMetrics {
    std::string& variant;                              // Reference to the test variant name
    bool         utflag;                               // Flag indicating whether this is a unit test
    int          exception_raised;                     // Floating-point exception values
    uint64_t     tcnt;                                 // Total number of test cases executed
    uint64_t     fcnt;                                 // Number of failed test cases
    double       max_ulp_err;                          // Maximum ULP (Units in the Last Place) error observed
    double       min_time;                             // Minimum execution time recorded
    double       max_time;                             // Maximum execution time recorded
    double       median_time;                          // Median execution time
    double       mean_time;                            // Mean (average) execution time
    double       stddev;                               // Standard deviation of execution times
    double       cov;                                  // Coefficient of variation (stddev / mean_time)
    double       mops;                                 // Million operations per second (performance metric)
    uint64_t     count_above_avg;                      // Number of runs with execution time above the average

    // Constructor to initialize the structure with a reference to the variant name
    PrintMetrics(std::string& var)
        : variant(var), utflag(false), tcnt(0), fcnt(0), max_ulp_err(0.0),
          min_time(0.0), max_time(0.0), median_time(0.0), mean_time(0.0),
          stddev(0.0), cov(0.0), mops(0.0), count_above_avg(0) {}
};

void print_table_header(bool nutflag);
void print_metrics(const PrintMetrics* pm);
