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
#include "platform.h"
#include "alm_test.h"
#include "api.h"

/*
 * print_usage:
 * Prints usage instructions for the program.
 */
void print_usage(const char *program_name)
{
    std::cerr << "Usage: " << program_name
              << " <shim_shared_library> <yaml_file> [--type|-t <test_type>]" << std::endl;
    std::cerr << "Description:\n"
              << "  This program loads shared libraries and executes specified functions.\n"
              << "  Arguments:\n"
              << "    <shim_shared_library>    Path to the shim shared library.\n"
              << "    <yaml_file>              Path to the YAML configuration file.\n"
              << "    [--type|-t <test_type>]  Optional test type: 'accu' for accuracy (default), 'perf' for performance.\n"
              << "  Note:\n"
              << "    The reference library is automatically loaded at build time." << std::endl;
}

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
    std::string shimlib   = argv[1];
    std::string yaml_file = argv[2];
    std::string reflib    = REF_MPARITH;  // Use the hardcoded reference library
    struct AlmLibs almlibs;
    TestMode test_mode = TestMode::E_ACCURACY;

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

    /* Read YAML file and populate test parameters */
    read_yaml_file(yaml_file, params);

    /* Execute tests using loaded libraries */
    process_libm(almlibs, params, test_mode);

    /* Clean up */
    DL_CLOSE(pshimobj);
    DL_CLOSE(prefobj);

    return 0;
}
