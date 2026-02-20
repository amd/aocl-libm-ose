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
#include <filesystem>
#include <cctype>
#include "alm_test.h"
#include "libm_yaml.h"
#include <cstdio>
#include <functional>

// Global filters (empty = no filter)
static std::string g_api_filter;
static std::string g_type_filter; // "UT", "VT", "PERF" or empty

// Setters used by main.cc
void set_api_filter(const std::string& api)
{
    g_api_filter = api;
}

void set_type_filter(const std::string& type)
{
    if (type == "CONF" || type == "ACCU" || type == "PERF" ||
        type == "conf" || type == "accu" || type == "perf")
        g_type_filter = type;
    else
        g_type_filter.clear();
}
// Utility: first path segment equals API (acos/..., tanh/...)
static bool firstSegmentEquals(const std::string& rel, const std::string& api) {
    if (api.empty()) return true;
    if (rel.empty()) return false;
    std::string s = rel;
    for (char& c : s) if (c == '\\') c = '/';
    auto pos = s.find('/');
    std::string head = (pos == std::string::npos) ? s : s.substr(0, pos);
    return head == api;
}

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
                range.type = type ? type[0].as<std::string>()
                                  : "expstep";
            } else {
                range.type = (type && type[n])
                    ? type[n].as<std::string>()
                    : type[n - 1].as<std::string>();
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
         "unittest_", "vt_", " "
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
    // If no prefix found, check if ID starts with function name followed by underscore and digit
    // This handles cases like "log2_1", "cos_2", etc.
    size_t firstUnderscore = id.find('_');
    if (firstUnderscore != std::string::npos && firstUnderscore > 0) {
        // Check if what follows the underscore is a digit (indicating a test number)
        if (firstUnderscore + 1 < id.length() && std::isdigit(id[firstUnderscore + 1])) {
            return id.substr(0, firstUnderscore);
        }
    }

     return id; // Return original if no prefix found
}

/*
 * parse_yaml_content:
 * Process the actual test content from a YAML node.
 */
static int parse_yaml_content(const YAML::Node &config,
                              std::vector<struct YamlInputs> &params)
{
    // Helper to process a single test_sequence-like map block
    auto process_block = [&](const YAML::Node& seq) -> int {
        if (!seq || !seq.IsMap()) return 0;

        // Require test_sets key; test_sequence/function are optional
        const YAML::Node test_sets = seq["test_sets"];
        if (!test_sets || !test_sets.IsSequence()) return 0;

        std::string test_type = seq["test_sequence"]
            ? seq["test_sequence"].as<std::string>() : "";
        std::string function = seq["function"]
            ? seq["function"].as<std::string>() : "";

        // Support generic.yml using “function: allApis” by inferring from each test’s id
        const bool inferApiFromId = (function == "allApis");

        for (std::size_t j = 0; j < test_sets.size(); j++) {
            const YAML::Node test_set = test_sets[j];
            if (!test_set || !test_set.IsMap()) continue;

            // Prefer "tests" array; if missing, synthesize from inline maps having "id"
            YAML::Node tests = test_set["tests"];
            if (!tests || !tests.IsSequence()) {
                YAML::Node synthetic = YAML::Load("[]");
                for (auto it = test_set.begin(); it != test_set.end(); ++it) {
                    if (it->first.IsScalar()) {
                        std::string key = it->first.as<std::string>();
                        if (key == "id" || key == "description" || key == "tests") continue;
                    }
                    const YAML::Node candidate = it->second;
                    if (candidate && candidate.IsMap() && candidate["id"]) {
                        synthetic.push_back(candidate);
                    }
                }
                tests = synthetic;
            }

            for (std::size_t k = 0; k < tests.size(); k++) {
                const YAML::Node test = tests[k];
                if (!test || !test.IsMap() || !test["id"]) continue;

                YamlInputs param;
                std::string id = test["id"].as<std::string>();

                // Derive API name: honor “function” unless it is “allApis”
                std::string apiName = inferApiFromId ? extractApiName(id)
                                                     : (function.empty() ? extractApiName(id) : function);

                param.api_name  = apiName;
                param.test_type = test_type;

                read_test(test, param);
                params.push_back(param);
            }
        }
        return 0;
    };

    // Recursive walk: process any map containing test_sets and traverse children
    std::function<int(const YAML::Node&)> walk = [&](const YAML::Node& node) -> int {
        if (!node) return 0;
        if (node.IsMap()) {
            if (node["test_sets"]) {
                int rc = process_block(node);
                if (rc != 0) return rc;
            }
            for (auto it = node.begin(); it != node.end(); ++it) {
                int rc = walk(it->second);
                if (rc != 0) return rc;
            }
            return 0;
        }
        if (node.IsSequence()) {
            for (std::size_t i = 0; i < node.size(); ++i) {
                int rc = walk(node[i]);
                if (rc != 0) return rc;
            }
            return 0;
        }
        return 0;
    };

    return walk(config);
}


// Detect master API map: keys are API names; values are null or sequence of file strings
static bool is_api_map(const YAML::Node& n) {
    if (!n || !n.IsMap()) return false;
    if (n["test_sequence"] && n["function"] && n["test_sets"]) return false;
    for (auto it = n.begin(); it != n.end(); ++it) {
        if (!it->first.IsScalar()) return false;
        const YAML::Node v = it->second;
        if (v && !v.IsSequence() && !v.IsNull()) return false;
        if (v && v.IsSequence()) {
            for (const auto& e : v) {
                if (!e.IsScalar()) return false;
            }
        }
    }
    return true;
}

// Classify test file by name for debug prints (correct suffix lengths)
static const char* classify_test_file(const std::string& path) {
    if (path.size() >= 9 && path.rfind("_conf.yml")   == path.size() - 9) return "CONF";
    if (path.size() >= 9 && path.rfind("_accu.yml")   == path.size() - 9) return "ACCU";
    if (path.size() >= 9 && path.rfind("_perf.yml") == path.size() - 9) return "PERF";
    return "TEST";
}

// Type filter based on filename suffix
static bool typeMatches(const std::string& path) {
    if (g_type_filter.empty()) return true;
    const char* kind = classify_test_file(path);
    return g_type_filter == kind;
}

static int handle_includes(const std::string &filename, std::vector<struct YamlInputs> &params)
{
    try {
        std::fprintf(stderr, "[yaml_read] open: %s\n", filename.c_str());
        YAML::Node config = YAML::LoadFile(filename);
        std::fprintf(stderr, "[yaml_read] loaded: %s\n", filename.c_str());

        std::filesystem::path current_file(filename);
        std::string base_dir = current_file.parent_path().string();

        // Case 1: legacy include list
        if (config["include"]) {
            std::fprintf(stderr, "[yaml_read] include-list master: %s\n", filename.c_str());
            for (const auto& include_entry : config["include"]) {
                const std::string include_file = include_entry.as<std::string>();

                // Apply API filter (first path segment must match)
                if (!firstSegmentEquals(include_file, g_api_filter)) {
                    std::fprintf(stderr, "  [yaml_read] skip by API filter=%s: %s\n",
                                 g_api_filter.c_str(), include_file.c_str());
                    continue;
                }

                std::filesystem::path full_include_path =
                    (!include_file.empty() && include_file[0] == '/')
                        ? std::filesystem::path(include_file)
                        : std::filesystem::path(base_dir) / include_file;

                const std::string full = full_include_path.string();

                // Apply type filter (UT/VT/PERF by filename suffix)
                if (!typeMatches(full)) {
                    std::fprintf(stderr, "  [yaml_read] skip by TYPE filter=%s: %s\n",
                                 g_type_filter.c_str(), full.c_str());
                    continue;
                }

                const char* kind = classify_test_file(full);
                std::fprintf(stderr, "  [yaml_read] Loading (%s): %s\n", kind, full.c_str());

                if (!std::filesystem::exists(full_include_path)) {
                    std::fprintf(stderr, "  [yaml_read] Missing: %s\n", full.c_str());
                    return -1;
                }
                int result = handle_includes(full, params);
                if (result != 0) {
                    std::fprintf(stderr, "  [yaml_read] Error processing included file: %s\n", full.c_str());
                    return result;
                }
            }
            return 0;
        }

        // Case 2: API map (keys are API names)
        if (is_api_map(config)) {
            std::fprintf(stderr, "[yaml_read] api-map master: %s\n", filename.c_str());
            for (auto it = config.begin(); it != config.end(); ++it) {
                const std::string api = it->first.as<std::string>();
                const YAML::Node node = it->second;

                // Apply API filter
                if (!g_api_filter.empty() && api != g_api_filter) {
                    std::fprintf(stderr, "  [yaml_read] skip API=%s (filter=%s)\n",
                                 api.c_str(), g_api_filter.c_str());
                    continue;
                }

                std::vector<std::filesystem::path> filesToLoad;

                if (node && node.IsSequence()) {
                    // Explicit per-API list
                    for (const auto& entry : node) {
                        const std::string rel = entry.as<std::string>();
                        std::filesystem::path p =
                            (!rel.empty() && rel[0] == '/')
                                ? std::filesystem::path(rel)
                                : std::filesystem::path(base_dir) / rel;
                        filesToLoad.push_back(p);
                    }
                } else {
                    // Auto defaults
                    std::filesystem::path apiDir = std::filesystem::path(base_dir) / api;
                    for (const auto& name : { api + "_conf.yml", api + "_accu.yml", api + "_perf.yml" }) {
                        filesToLoad.push_back(apiDir / name);
                    }
                }

                int loadedCount = 0;
                for (const auto& p : filesToLoad) {
                    const std::string full = p.string();

                    // Apply type filter
                    if (!typeMatches(full)) {
                        std::fprintf(stderr, "  [yaml_read] skip by TYPE filter=%s: %s\n",
                                     g_type_filter.c_str(), full.c_str());
                        continue;
                    }

                    const char* kind = classify_test_file(full);
                    std::fprintf(stderr, "  [yaml_read] Loading (%s): %s\n", kind, full.c_str());

                    if (!std::filesystem::exists(p)) {
                        std::fprintf(stderr, "  [yaml_read] Missing: %s\n", full.c_str());
                        continue;
                    }
                    int result = handle_includes(full, params);
                    if (result != 0) {
                        std::fprintf(stderr, "  [yaml_read] Error processing included file: %s\n", full.c_str());
                        return result;
                    }
                    loadedCount++;
                }

                if (loadedCount == 0) {
                    std::fprintf(stderr, "  [yaml_read] Error: No test files found for API: %s\n", api.c_str());
                    return -1;
                }
            }
            return 0;
        }

        // Case 3: test file
        const char* kind = classify_test_file(filename);
        std::fprintf(stderr, "[yaml_read] Processing test file (%s): %s\n", kind, filename.c_str());
        int rc = parse_yaml_content(config, params);
        std::fprintf(stderr, "[yaml_read] Done test file (%s): %s rc=%d\n", kind, filename.c_str(), rc);
        return rc;

    } catch (const YAML::Exception& e) {
        std::fprintf(stderr, "[yaml_read] YAML error in %s: %s\n", filename.c_str(), e.what());
        return -1;
    } catch (const std::filesystem::filesystem_error& e) {
        std::fprintf(stderr, "[yaml_read] File system error: %s\n", e.what());
        return -1;
    }
}
/*
 * read_yaml_file:
 * Load a YAML file and populate a vector of YamlInputs with parsed test cases.
 */
int read_yaml_file(const std::string &filename, std::vector<struct YamlInputs> &params)
{
    return handle_includes(filename, params);
}