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

#include <iostream>
#include <cstring>
#include <map>
#include "dll_utils.h"
#include "alm_test.h"
#include "api_template.h"
#include <string>

// Helper to uppercase a string
static std::string to_upper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::toupper(c));
        });
    return s;
}

/*
 * print_usage:
 * Prints usage instructions for the program.
 */
void print_usage(const char *program_name)
{
    std::cerr << "Usage: " << program_name
              << " <shim_shared_library> <yaml_file>"
              << " [--type|-t <test_type>]" << std::endl;
    std::cerr << "Description:\n"
              << "  This program loads shared libraries and "
              << "executes specified functions.\n"
              << "  Arguments:\n"
              << "    <shim_shared_library>    Path to the shim "
              << "shared library.\n"
              << "    <yaml_file>              Path to the YAML "
              << "configuration file.\n"
              << "    [--type|-t <test_type>]  Optional test type:\n"
              << "                             'accu' for accuracy "
              << "(default),\n"
              << "                             'perf' for "
              << "performance.\n"
              << "  Note:\n"
              << "    The reference library is automatically loaded "
              << "at build time." << std::endl;
}

void set_api_filter(const std::string& api);

/*
 * main:
 * Entry point for the test runner.
 */
int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 5) {
        print_usage(argv[0]);
        return 1;
    }

    std::vector<YamlInputs> params;
    std::string shimlib   = std::string(argv[1]);
    std::string yaml_file = std::string(argv[2]);

    std::string api_name; // optional

    // Force stderr to be unbuffered for immediate visibility
    setvbuf(stderr, nullptr, _IONBF, 0);
    std::fprintf(stderr, "[%s] starting. argc=%d\n", argv[0], argc);
    TestMode test_mode = TestMode::E_ACCURACY;

    // Auto-detect test_mode from yaml filename if not explicitly set
    if (yaml_file.find("_perf.yml") != std::string::npos) {
        test_mode = TestMode::E_PERFORMANCE;
        std::fprintf(stderr,
            "[%s] Auto-detected PERFORMANCE mode "
            "from filename\n", argv[0]);
    } else if (yaml_file.find("_accu.yml") != std::string::npos) {
        test_mode = TestMode::E_ACCURACY;
        std::fprintf(stderr,
            "[%s] Auto-detected ACCURACY mode "
            "from filename\n", argv[0]);
    } else if (yaml_file.find("_conf.yml") != std::string::npos) {
        test_mode = TestMode::E_ACCURACY;
        std::fprintf(stderr,
            "[%s] Auto-detected ACCURACY mode "
            "from filename (conf)\n", argv[0]);
    }

 // Arg3 may be API or TYPE (case-insensitive for type)
    if (argc >= 4 && std::strlen(argv[3]) > 0 && argv[3][0] != '-') {
        std::string t3 = to_upper(argv[3]);
        if (t3 == "CONF" || t3 == "ACCU" || t3 == "PERF") {
            set_type_filter(t3);
            set_api_filter("");  // no API filter when type only
            std::fprintf(stderr,
                "[%s] TYPE filter: %s\n", argv[0], t3.c_str());
            // Set test_mode based on type filter
            if (t3 == "PERF")
                test_mode = TestMode::E_PERFORMANCE;
            else if (t3 == "ACCU")
                test_mode = TestMode::E_ACCURACY;
        } else {
            set_api_filter(argv[3]);
            std::fprintf(stderr,
                "[%s] API filter: %s\n", argv[0], argv[3]);
        }
    }

    // Arg4 is TYPE when both API and TYPE are provided
    if (argc >= 5 && std::strlen(argv[4]) > 0 && argv[4][0] != '-') {
        std::string t4 = to_upper(argv[4]);
        if (t4 == "CONF" || t4 == "ACCU" || t4 == "PERF") {
            set_type_filter(t4);
            std::fprintf(stderr,
                "[%s] TYPE filter: %s\n", argv[0], t4.c_str());
            // Set test_mode based on type filter
            if (t4 == "PERF")
                test_mode = TestMode::E_PERFORMANCE;
            else if (t4 == "ACCU")
                test_mode = TestMode::E_ACCURACY;
        } else {
            std::fprintf(stderr,
                "[%s] Warning: unknown type arg: %s "
                "(ignored)\n", argv[0], argv[4]);
        }
    }

    std::string reflib    = REF_LIB;  // Use the hardcoded reference library
    struct AlmLibs almlibs = {};

    /* Parse optional arguments */
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--type") == 0 ||
            std::strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) {
                std::string test_type = argv[++i];
                if (test_type == "accu") {
                    test_mode = TestMode::E_ACCURACY;
                } else if (test_type == "perf") {
                    test_mode = TestMode::E_PERFORMANCE;
                } else {
                    test_mode = TestMode::E_ACCURACY;
                }
            }
        }
    }

    /* Load shim shared library */
    DL_HANDLE pshimobj = DL_LOAD(shimlib.c_str());
    if (!pshimobj) {
        std::cerr << "Error loading the shim library " << shimlib
                  << " - " << DL_ERROR() << std::endl;
        return -1;
    }
    DL_ERROR();

#ifdef _WIN32
    char dllPath[MAX_PATH];
    if (GetModuleFileNameA(pshimobj, dllPath, MAX_PATH)) {
        std::cout << "Shim DLL loaded from   : " << dllPath << std::endl;
    } else {
        std::cerr << "Failed to get shim DLL path. Error: " << GetLastError() << std::endl;
    }
#else
    std::cout << "Shim library loaded   : " << shimlib << std::endl;
#endif
    std::cout << "Mparith library: " << reflib.c_str() << std::endl;
    DL_HANDLE prefobj = DL_LOAD(reflib.c_str());
    if (!prefobj) {
        std::cerr << "Error loading the Mparith reference library - " << DL_ERROR() << std::endl;
        return -1;
    }
    DL_ERROR();

#ifdef _WIN32
    if (GetModuleFileNameA(prefobj, dllPath, MAX_PATH)) {
        std::cout << "Mparith DLL loaded from: " << dllPath << std::endl;
    } else {
        std::cerr << "Failed to get Mparith DLL path. Error: " << GetLastError() << std::endl;
    }
#else
    std::cout << "Mparith library loaded: " << reflib << std::endl;
#endif

    almlibs.pshimlib = pshimobj;
    almlibs.preflib  = prefobj;

    // ...existing code...
    std::fprintf(stderr, "[%s] reading YAML: %s\n", argv[0], yaml_file.c_str());

    /* Read YAML file and populate test parameters */
    read_yaml_file(yaml_file, params);

    /* Execute tests using loaded libraries */
    process_libm(&almlibs, params, test_mode);

    /* Clean up */
    DL_CLOSE(pshimobj);
    DL_CLOSE(prefobj);

    return 0;
}