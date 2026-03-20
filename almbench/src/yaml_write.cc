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


#include <sstream>
#include <iomanip>
#include <cstdint>

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <cfenv>  // For floating-point exception handling

// Platform-specific includes
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #pragma warning(push)
    #pragma warning(disable: 4996)  // Disable deprecated function warnings
    #include <windows.h>
    #include <io.h>
    #define popen _popen
    #define pclose _pclose
#else
    #include <sys/utsname.h>
    #include <unistd.h>
    #include <dlfcn.h>
#endif

#include "alm_test.h"
#include "libm_yaml.h"

#ifdef _WIN32
    #pragma warning(pop)  // Restore warnings
#endif

/*
 * Converts floating-point exception flags into a human-readable string.
 * This is useful for debugging or logging exception states.
 */
static std::string exception_to_string(int raised_exception)
{
    std::string result;

    if (raised_exception & FE_DIVBYZERO)
        result += "FE_DIVBYZERO ";
    if (raised_exception & FE_INEXACT)
        result += "FE_INEXACT ";
    if (raised_exception & FE_INVALID)
        result += "FE_INVALID ";
    if (raised_exception & FE_OVERFLOW)
        result += "FE_OVERFLOW ";
    if (raised_exception & FE_UNDERFLOW)
        result += "FE_UNDERFLOW ";

    // Remove trailing space if any exceptions were added
    if (!result.empty())
        result.pop_back();

    return result;
}

/*
 * Converts a floating-point value to its hexadecimal bit representation.
 * This is useful for precise comparison and serialization of floating-point data.
 */
template <typename S>
static std::string to_hex(const S &value)
{
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setfill('0');
    if (sizeof(S) == sizeof(uint32_t)) {
        uint32_t bits = *(reinterpret_cast<const uint32_t *>(&value));
        ss << std::setw(8) << bits;
    } else if (sizeof(S) == sizeof(uint64_t)) {
        uint64_t bits = *(reinterpret_cast<const uint64_t *>(&value));
        ss << std::setw(16) << bits;
    }
    return ss.str();
}

/*
 * Serializes the contents of a YamlOutputs structure into a YAML node.
 * This function handles both unit test mode (utflag = true) and batch mode.
 */
template <typename S>
YAML::Node serialize_yaml_outputs(const struct YamlOutputs<S> *yop)
{
    YAML::Node node;

    // Use compact flow style for top-level node
    node.SetStyle(YAML::EmitterStyle::Flow);

    // Basic metadata
    node["api"] = yop->api_name + "_" +yop->variant;
    node["n"] = yop->n[0];
    node["test_id"] = yop->test_id;

    // Serialize input pointers
    for (int i = 0; i < MAX_IPPTR; ++i) {
        if (yop->iptr[i]) {
            if (yop->utflag) {
                // Unit test mode: only serialize the first element
                YAML::Node ip_node;
                ip_node.push_back(to_hex(yop->iptr[i][0]));
                ip_node.SetStyle(YAML::EmitterStyle::Flow);
                std::string key = "ip" + std::to_string(i + 1);
                node[key] = ip_node;
            } else {
                // Batch mode: serialize all elements as hex
                YAML::Node ip_node;
                YAML::Node inner_list;
                for (uint64_t j = 0; j < yop->n[i]; ++j) {
                    inner_list.push_back(to_hex(yop->iptr[i][j]));
                }
                inner_list.SetStyle(YAML::EmitterStyle::Flow);
                ip_node.push_back(inner_list);
                std::string key = "ip" + std::to_string(i + 1);
                node[key] = ip_node;
            }
        }
    }

    // Serialize yop pointers
    for (int i = 0; i < MAX_OPPTR; ++i) {
        if (yop->optr[i]) {
            if (yop->utflag) {
                YAML::Node op_node;
                op_node.push_back(to_hex(yop->optr[i][0]));
                op_node.SetStyle(YAML::EmitterStyle::Flow);
                std::string key = "op" + std::to_string(i + 1);
                node[key] = op_node;
            } else {
                YAML::Node op_node;
                YAML::Node inner_list;
                for (uint64_t j = 0; j < yop->n[0]; ++j) {
                    inner_list.push_back(to_hex(yop->optr[i][j]));
                }
                inner_list.SetStyle(YAML::EmitterStyle::Flow);
                op_node.push_back(inner_list);
                std::string key = "op" + std::to_string(i + 1);
                node[key] = op_node;
            }
        }
    }

    // Serialize ULP (Unit in Last Place) values
    if (yop->ulp) {
        if (yop->utflag) {
            YAML::Node ulp_node;
            ulp_node.push_back(to_hex(yop->ulp[0]));
            ulp_node.SetStyle(YAML::EmitterStyle::Flow);
            node["ulp"] = ulp_node;
        } else {
            YAML::Node ulp_inner;
            for (uint64_t j = 0; j < yop->n[0]; ++j) {
                ulp_inner.push_back(to_hex(yop->ulp[j]));
            }
            ulp_inner.SetStyle(YAML::EmitterStyle::Flow);
            YAML::Node ulp_outer;
            ulp_outer.push_back(ulp_inner);
            node["ulp"] = ulp_outer;
        }
    }


    // Serialize ULP (Unit in Last Place) values
    if (yop->status) {
        if (yop->utflag) {
            YAML::Node status_node;
            status_node.push_back(yop->status[0]==TESTCASE_PASS ?"PASS":"FAIL");
            status_node.SetStyle(YAML::EmitterStyle::Flow);
            node["status"] = status_node;
        } else {
            YAML::Node status_inner;
            for (uint64_t j = 0; j < yop->n[0]; ++j) {
                status_inner.push_back(yop->status[j]==TESTCASE_PASS ?"PASS":"FAIL");
            }
            status_inner.SetStyle(YAML::EmitterStyle::Flow);
            YAML::Node status_outer;
            status_outer.push_back(status_inner);
            node["status"] = status_outer;
        }
    }

    // Serialize either raised exceptions or duration depending on mode
    if (yop->utflag) {
        std::string except = exception_to_string(yop->exception_raised);
        YAML::Node expt_node;
        expt_node.push_back(except);
        expt_node.SetStyle(YAML::EmitterStyle::Flow);
        node["exception_raised"] = expt_node;
    } else {
        if(yop->test_mode == TestMode::E_PERFORMANCE) {
            YAML::Node duration_node;
            duration_node.push_back(yop->duration);
            duration_node.SetStyle(YAML::EmitterStyle::Flow);
            node["duration"] = duration_node;
        }
    }

    // Print YAML to stdout in unit test mode
    if (yop->utflag) {
        YAML::Emitter out;
        out << node;
        std::cout << out.c_str() << std::endl;
    }

    return node;
}

/*
 * Writes the serialized YAML output to a file.
 * Appends to the file if it already exists.
 */
template <typename S>
void write_yaml_output(const struct YamlOutputs<S> *yop)
{
    YAML::Node node = serialize_yaml_outputs<S>(yop);

    std::ofstream fout(yop->outfile, std::ios::app);
    if (!fout.is_open()) {
        std::cerr << "Error: Could not open file " << yop->outfile << " for writing." << std::endl;
        return;
    }

    fout << node << "\n";
    fout.close();
}


// Explicit template instantiations for float and double types
template void write_yaml_output<float>(const struct YamlOutputs<float> *yop);
template void write_yaml_output<double>(const struct YamlOutputs<double> *yop);

#if 0
template void populate_system_metadata<float>(struct TestMetadata<float> *metadata);
template void populate_system_metadata<double>(struct TestMetadata<double> *metadata);

template struct TestMetadata<float>* get_metadata<float>();
template struct TestMetadata<double>* get_metadata<double>();

template void del_metadata<float>(struct TestMetadata<float>* metadata);
template void del_metadata<double>(struct TestMetadata<double>* metadata);

template void write_test_metadata<float>(const struct TestMetadata<float> *metadata, const std::string &filename);
template void write_test_metadata<double>(const struct TestMetadata<double> *metadata, const std::string &filename);

/*
 * Helper function to execute a system command and return its output
 */
static std::string exec_command(const char* cmd) {
    char buffer[128];
    std::string result = "";

    FILE* pipe = popen(cmd, "r");

    if (pipe) {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
    }

    // Remove trailing newline if present
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

/*
 * Helper function to read file content
 */
static std::string read_file_content(const std::string& filename) {
    std::ifstream file(filename);
    if (file) {
        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        // Remove trailing newline if present
        if (!content.empty() && content.back() == '\n') {
            content.pop_back();
        }
        return content;
    }
    return "unknown";
}

/*
 * Populates TestMetadata structure with actual system information
 */
template <typename S>
void populate_system_metadata(struct TestMetadata<S> *metadata) {
    if (!metadata) return;

    // Get current timestamp for build date
    std::time_t now = std::time(nullptr);
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    metadata->build_date = std::string(timestamp);

    // Get build version (try to get from git or use default)
    std::string build_version = exec_command("git describe --tags --always --dirty 2>/dev/null");
    if (build_version.empty()) {
        build_version = "dev-build";
    }
    metadata->build_version = build_version;

#ifdef _WIN32
    // Get CPU information from registry or environment
    char* processor = std::getenv("PROCESSOR_IDENTIFIER");
    if (processor) {
        metadata->cpu_info = std::string(processor);
    } else {
        metadata->cpu_info = "Intel/AMD x64";
    }

#else
    // Get CPU information from /proc/cpuinfo
    std::string cpu_info = exec_command("grep 'model name' /proc/cpuinfo | head -1 | cut -d':' -f2 | sed 's/^ *//' 2>/dev/null");
    if (!cpu_info.empty()) {
        metadata->cpu_info = cpu_info;
    } else {
        // Fallback to lscpu
        cpu_info = exec_command("lscpu | grep 'Model name' | cut -d':' -f2 | sed 's/^ *//' 2>/dev/null");
        if (!cpu_info.empty()) {
            metadata->cpu_info = cpu_info;
        } else {
            metadata->cpu_info = "Unknown CPU";
        }
    }

#endif

    // Library information
    metadata->library_name = "AMD Optimized CPU Libraries - LibM";

    // Try to get library version from build system or version file
    std::string lib_version = read_file_content("src/version.txt");
    if (lib_version == "unknown") {
        lib_version = exec_command("grep 'VERSION' CMakeLists.txt | head -1 | awk '{print $2}' 2>/dev/null");
        if (lib_version.empty()) {
            lib_version = "4.2.0"; // Default fallback version
        }
    }
    metadata->library_version = lib_version;

    // Test execution environment
    metadata->test_runner = "libm_testsuite";

    if (metadata->count == 0) {
        metadata->count = 1000; // Default test count
    }

    if (metadata->input_range.empty()) {
        metadata->input_range = "default";
    }
}

/*
 * Creates and returns a TestMetadata structure with system information
 */
template <typename S>
struct TestMetadata<S>* get_metadata() {
    struct TestMetadata<S>* metadata = new TestMetadata<S>();
    populate_system_metadata<S>(metadata);
    return metadata;
}

template <typename S>
void del_metadata(struct TestMetadata<S>* metadata) {
    if (metadata) {
        delete metadata;
    }
}

/*
 * Writes TestMetadata structure to a YAML file
 */
template <typename S>
void write_test_metadata(const struct TestMetadata<S> *metadata, const std::string &filename) {

    if (!metadata) return;

    YAML::Node node;

    // System and build information
    node["test_suite_name"] = metadata->test_suite_name;
    node["test_version"] = metadata->test_version;

    // Library information
    node["library_name"] = metadata->library_name;
    node["library_version"] = metadata->library_version;
    node["library"] = metadata->library;

    // Test execution information
    node["test_runner"] = metadata->test_runner;

    // Test-specific information
    node["api_name"] = metadata->api_name;
    node["test_id"] = metadata->test_id;
    node["description"] = metadata->description;
    node["test_type"] = metadata->test_type;
    node["variant"] = metadata->variant;
    node["input_range"] = metadata->input_range;
    node["count"] = metadata->count;
    node["threshold"] = metadata->threshold;

    // Write to file
    std::ofstream fout(filename);
    if (fout.is_open()) {
        fout << "# Test Metadata\n";
        fout << "---\n";
        fout << node << "\n";
        fout.close();
    } else {
        std::cerr << "Error: Could not open metadata file " << filename << " for writing." << std::endl;
    }
}
#endif