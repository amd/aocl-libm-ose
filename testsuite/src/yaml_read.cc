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

#include <yaml-cpp/yaml.h>
#include "alm_test.h"
#include "libm_yaml.h"

/*
 * read_test:
 * Parse a single test case from a YAML node and populate the YamlInputs struct.
 * Handles both single-value inputs and input ranges.
 */
static int read_test(const YAML::Node &test, struct YamlInputs &param)
{
    std::string test_id = test["id"].as<std::string>();

    /* Optional description field (not stored) */
    if (test["description"]) {
        std::string description = test["description"].as<std::string>();
    }

    const YAML::Node input = test["input"];
    const YAML::Node expect = test["expect"];
    const YAML::Node type = test["type"];
    const YAML::Node steps = test["steps"];
    const YAML::Node variants = test["variants"];
    const YAML::Node exp_excep = test["expect_exception"];
    const YAML::Node uth = test["uth"];

    uint32_t n = 0;
    param.test_id = test_id;

    /*
     * Process input values:
     * - If the input is a sequence, treat it as a range [start, stop]
     * - Otherwise, treat it as a single value
     */
    for (std::size_t i = 0; i < input.size(); i++) {
        const YAML::Node inp = input[i];

        if (inp.IsSequence()) {
            struct InputRange range;

            range.srt = inp[0].as<std::string>();
            range.stp = inp[1].as<std::string>();

            if (n == 0) {
                range.type = type ? type[0].as<std::string>() : "expstep";
            } else {
                range.type = (type && type[n]) ? type[n].as<std::string>() : type[n - 1].as<std::string>();
            }

            range.count = steps ? steps[n].as<std::string>() : "1000";

            param.range.push_back(range);
            n++;
        } else {
            param.input.push_back(inp.as<std::string>());
        }
    }

    /* Read expected output value if present */
    if (expect) {
        param.xv = expect[0].as<std::string>();
    }

    /* Read variants and concatenate them with semicolons */
    if (variants) {
        std::string vrts, vr;

        for (std::size_t i = 0; i < variants.size(); i++) {
            vrts += variants[i].as<std::string>() + ";";
        }

        if (!vrts.empty()) {
            vrts.pop_back(); /* Remove trailing semicolon */
        }

        param.variants = vrts;
    }

    /* Read expected exception if present */
    if (exp_excep) {
         std::string str;
         for (std::size_t i = 0; i < exp_excep.size(); i++)
         {

            str += exp_excep[i].as<std::string>() + ";";
         }
        if (!str.empty())
        {
            str.pop_back(); /* Remove trailing semicolon */
        }
        param.xxv = str;
    }

    /* Read ULP threshold if present */
    if (uth) {
        std::string thold, ut, vr;

        for (std::size_t i = 0; i < uth.size(); i++) {
            ut = uth[i].as<std::string>() ;
            vr = variants[i].as<std::string>() ;
            if(vr == "ss" || vr == "sd") {
                if(ut.empty()) {
                    ut = "0.5";
                }
            } else {
                if(ut.empty()) {
                    ut = "4.0"; /* Default ULP threshold for other variants */
                }
            }
            thold = thold + ut +  ";" ;
        }

        if (!thold.empty()) {
            thold.pop_back(); /* Remove trailing semicolon */
        }

        param.ulp_threshold = thold;
    }
    else {
        std::string thold, ut, vr;

        for (std::size_t i = 0; i < variants.size(); i++) {
            vr = variants[i].as<std::string>() ;
            if(vr == "ss" || vr == "sd") {
                ut = "0.5";
            } else {
                ut = "4.0"; /* Default ULP threshold for other variants */
            }
            thold = thold + ut +  ";" ;
        }

        if (!thold.empty()) {
            thold.pop_back(); /* Remove trailing semicolon */
        }

        param.ulp_threshold = thold;
    }
    return 0;
}

std::string extractApiName(const std::string& id) {
   std::vector<std::string> prefixes = {
        "accu_", "perf_", "conf_",
         "unittest_", "vt_"
   };

    // Check for known prefixes
    for (const auto& prefix : prefixes) {
        if (id.length() > prefix.length() &&
            id.substr(0, prefix.length()) == prefix) {

            std::string remaining = id.substr(prefix.length());

            // Find next underscore to separate API name from suffix
            size_t nextUnderscore = remaining.find('_');
            if (nextUnderscore != std::string::npos) {
                return remaining.substr(0, nextUnderscore);
            }
            return remaining;
        }
    }
     return id; // Return original if no prefix found
}

/*
 * read_yaml_file:
 * Load a YAML file and populate a vector of YamlInputs with parsed test cases.
 */
int read_yaml_file(const std::string &filename, std::vector<struct YamlInputs> &params)
{
    YAML::Node config = YAML::LoadFile(filename);

    for (std::size_t i = 0; i < config.size(); i++) {
        const YAML::Node test_sequence = config[i];

        std::string test_type = test_sequence["test_sequence"].as<std::string>();
        std::string function = test_sequence["function"].as<std::string>();

        const YAML::Node test_sets = test_sequence["test_sets"];

        for (std::size_t j = 0; j < test_sets.size(); j++) {
            const YAML::Node test_set = test_sets[j];
            std::string test_set_id = test_set["id"].as<std::string>();

            if (test_set["description"]) {
                std::string description = test_set["description"].as<std::string>();
            }

            const YAML::Node tests = test_set["tests"];

            for (std::size_t k = 0; k < tests.size(); k++) {
                const YAML::Node test = tests[k];
                struct YamlInputs param;

                //param.api_name = function;
                std::string id = test["id"].as<std::string>();
                std::string apiName = extractApiName(id);
                param.api_name = apiName;
                param.test_type = test_set_id;

                read_test(test, param);

                params.push_back(param);
            }
        }
    }

    return 0;
}
