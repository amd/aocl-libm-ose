/*
 * Copyright (C) 2025-2026, Advanced Micro Devices. All rights reserved.
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
#include "dll_utils.h"
#include <cstring>
#include <regex>
#include <unordered_map>
#include <type_traits>
#include "alm_test.h"
#include "api_template.h"
#include "generator.h"

/*
 * is_hex_string:
 * Checks if a string is a valid hexadecimal representation.
 */
bool is_hex_string(const std::string &str)
{
    std::regex hex_pattern("^0[xX][0-9a-fA-F]+$");
    return std::regex_match(str, hex_pattern);
}

/*
 * is_decimal_string:
 * Checks if a string is a valid decimal or scientific notation.
 */
bool is_decimal_string(const std::string &str)
{
    std::regex decimal_pattern(R"(^-?\d+(\.\d+)?([eEpP][-+]?\d+)?$)");
    return std::regex_match(str, decimal_pattern);
}

/*
 * str2flt:
 * Converts a string to a floating-point value of type T.
 */
template <typename T>
bool str2flt(const std::string &value, T &rhs)
{
    if (value == "max") {
        rhs = std::numeric_limits<T>::max();
    } else if (value == "min") {
        rhs = std::numeric_limits<T>::min();
    } else if (value == "min_subnormal") {
        rhs = std::numeric_limits<T>::denorm_min();
    } else if (value == "max_subnormal") {
        rhs = (std::numeric_limits<T>::min() - std::numeric_limits<T>::denorm_min());
    } else if (value == "qnan") {
        rhs = std::numeric_limits<T>::quiet_NaN();
    } else if (value == "snan" || value == "nan") {
        rhs = std::numeric_limits<T>::signaling_NaN();
    } else if (value == "inf") {
        rhs = std::numeric_limits<T>::infinity();
    } else {
        // Try to convert the string to a float or double
        try {
            if constexpr (std::is_same<T, float>::value) {
                if (is_decimal_string(value)) {
                    rhs = std::stof(value);
                } else {
                    uint32_t hex_value = std::stoul(value, NULL, 16);
                    std::memcpy(&rhs, &hex_value, sizeof(float));
                }
            } else if constexpr (std::is_same<T, double>::value) {
                if (is_decimal_string(value)) {
                    rhs = std::stod(value);
                } else {
                    uint64_t hex_value = std::stoull(value, NULL, 16);
                    std::memcpy(&rhs, &hex_value, sizeof(double));
                }
            }
        } catch (const std::invalid_argument &) {
            return false;
        }
    }

    return true;
}

/*
 * str_to_float:
 * Wrapper to convert a string to a float or double, handling sign and case.
 */
template <typename T>
T str_to_float(const std::string &word)
{
    T value = {0.0};
    std::string str = word;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    bool is_negative = false;
    char sign = '-';
    //Remove the leading '-' sign if present
    if (!str.empty() && str[0] == sign) {
        str.erase(0, 1);
        is_negative = true;
    }

    if (!str2flt<T>(str, value)) {
        std::cerr << "Invalid string: " << word << std::endl;
    }

    if (is_negative) {
        value = -value;
    }

    return value;
}

/*
 * get_exception_flag:
 * Converts a string to a floating-point exception flag.
 */
int get_exception_flag(const std::string &name)
{
    static const std::unordered_map<std::string, int> exception_map = {
        {"divbyzero", FE_DIVBYZERO},
        {"invalidoperation", FE_INVALID},
        {"overflow", FE_OVERFLOW},
        {"underflow", FE_UNDERFLOW},
        {"inexact", FE_INEXACT}
    };

    int temp = 0;
    std::stringstream ss(name);
    std::string word;
    while (std::getline(ss, word, ';'))
    {
        std::string normalized;
        for (char ch : word)
        {
            if (ch != ' ' && ch != '_')
                normalized += std::tolower(static_cast<unsigned char>(ch));
        }
        std::cout << "Looking for exception: " << normalized << std::endl;
        auto it = exception_map.find(normalized);
        if (it != exception_map.end())
        {
            temp |= it->second;
        }
        else if (!normalized.empty())
        {
            throw std::invalid_argument("Unknown exception name: " + normalized);
        }
    }
    return temp;
}

/*
 * str_to_enum:
 * Converts a string to a RangeType enum.
 */
RangeType str_to_enum(const std::string &key)
{
    static const std::unordered_map<std::string, RangeType> range_type_map = {
        {"simple",      RangeType::E_Simple},
        {"integer",     RangeType::E_Integer},
        {"fixedval",    RangeType::E_Fixedval},
        {"random",      RangeType::E_Random},
        {"linearstep",  RangeType::E_Linear},
        {"expstep",     RangeType::E_Expstep},
        {"bitstep",     RangeType::E_Bitstep},
        {"derived",     RangeType::E_Derived},
        {"MAX",         RangeType::E_MAX}
    };

    auto it = range_type_map.find(key);
    if (it != range_type_map.end()) {
        return it->second;
    }

    std::cerr << key << " not found in range type map." << std::endl;
    return RangeType::E_MAX;
}

/*
 * str_to_complex:
 * Parses strings like "0x... + i 0x..." or decimal "1.0+ i 2.0" into C complex types.
 */
template <typename T>
static T str_to_complex(const std::string &word)
{
    using CT = typename libm::complex_component_type<T>::type;
    std::regex cre(R"(^\s*(.+?)\s*([+-])\s*i\s*(.+)\s*$)");
    std::smatch m;
    if (!std::regex_match(word, m, cre)) {
        std::cerr << "Invalid complex literal: " << word << std::endl;
        return T{};
    }
    std::string re_s = m[1].str();
    std::string im_s = m[3].str();
    /* Trim */
    auto trim = [](std::string &s) {
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) {
            s.erase(0, 1);
        }
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
            s.pop_back();
        }
    };
    trim(re_s);
    trim(im_s);
    CT re_v = str_to_float<CT>(re_s);
    CT im_v = str_to_float<CT>(im_s);
    if (m[2].str() == "-") {
        im_v = -im_v;
    }
    return libm::fc_pack(re_v, im_v);
}

/*
 * libm_api_variant:
 * Handles test execution for a specific API variant and input type.
 */
template <typename T>
void libm_api_variant(struct AlmLibs *alibs, const struct YamlInputs &param,
                      std::string &variant, TestMode test_mode, const std::string &vendor,
                      std::string &ulp_threshold)
{
    using U = typename libm::type_info<T>::real_type;

    struct InParams<T, U> *ipp = new InParams<T, U>;
    struct YamlOutputs<U> *yop = new YamlOutputs<U>(variant);

    yop->variant = variant;
    yop->api_name = param.api_name;
    yop->utflag = param.range.empty();
    yop->test_mode = test_mode;
    yop->vendor = vendor;
    yop->ulp_threshold = std::stod(ulp_threshold);
    yop->test_id = param.test_id;

    if (!param.range.empty()) {
        for (const auto &range : param.range) {
            struct InpRng<U> iprng;
            if constexpr (std::is_same_v<U, fc32_t> || std::is_same_v<U, fc64_t>) {
                iprng.srt = str_to_complex<U>(range.srt);
                iprng.stp = str_to_complex<U>(range.stp);
            } else {
                iprng.srt = str_to_float<U>(range.srt);
                iprng.stp = str_to_float<U>(range.stp);
            }
            iprng.type  = str_to_enum(range.type);
            iprng.count = std::stoull(range.count);
            if (iprng.type == RangeType::E_Derived && range.derived) {
                iprng.derived.emplace();
                iprng.derived->z_srt   = str_to_float<U>(range.derived->z_srt);
                iprng.derived->z_stp   = str_to_float<U>(range.derived->z_stp);
                iprng.derived->z_type  = str_to_enum(range.derived->z_type);
                iprng.derived->z_count = std::stoull(range.derived->z_count);
                iprng.derived->func    = range.derived->func;
            }
            ipp->range.push_back(iprng);
        }
        for (const auto &input : param.input) {
            struct InpRng<U> iprng;
            if constexpr (std::is_same_v<U, fc32_t> || std::is_same_v<U, fc64_t>) {
                U value = str_to_complex<U>(input);
                iprng.srt   = value;
                iprng.stp   = value;
            } else {
                U value = str_to_float<U>(input);
                iprng.srt   = value;
                iprng.stp   = value;
            }
            iprng.type  = RangeType::E_Fixedval; /* range-test with single input */
            iprng.count = 1;
            ipp->range.push_back(iprng);
        }
    } else {
        int i = 0;
        for (const auto &input : param.input) {
            if constexpr (std::is_same_v<U, fc32_t> || std::is_same_v<U, fc64_t>) {
                ipp->ip[i++] = str_to_complex<T>(input);
            } else {
                U inp = str_to_float<U>(input);
                ipp->ip[i++] = T{inp};
            }
        }
    }

    if (!param.xv.empty()) {
        if constexpr (std::is_same_v<U, fc32_t> || std::is_same_v<U, fc64_t>) {
            ipp->xv = str_to_complex<U>(param.xv);
        } else {
            ipp->xv = str_to_float<U>(param.xv);
        }
    }

    if (!param.xxv.empty()) {
        ipp->xxv = get_exception_flag(param.xxv);
    }

    validate_api<T, U>(alibs, ipp, yop);

    delete ipp;
    delete yop;
}

/*
 * process_libm:
 * Executes tests for each variant specified in the YAML input using the appropriate libm API variant.
 */
void process_libm(struct AlmLibs *alibs, const std::vector<struct YamlInputs> &params, TestMode test_mode)
{
    typedef const char* (*get_vendor_name_t)();
    get_vendor_name_t get_vendor_string = (get_vendor_name_t)DL_SYM(alibs->pshimlib, "get_vendor_name");
    const char *vendor_cstr = get_vendor_string();
    std::cout << "Vendor: " << vendor_cstr << std::endl;
    std::string vendor(vendor_cstr);
    std::transform(vendor.begin(), vendor.end(), vendor.begin(), ::tolower);

    for (const auto &param : params) {
        std::string variants = param.variants;
        std::stringstream ss(variants);
        std::string variant;
        std::string ulp_threshold = param.ulp_threshold;
        std::stringstream ulp(ulp_threshold);
        std::string uth;

        // Derive test_mode from test_type
        test_mode = (param.test_type.find("perf") != std::string::npos) ? TestMode::E_PERFORMANCE : TestMode::E_ACCURACY;

        while (std::getline(ss, variant, ';')) {
            std::getline(ulp, uth, ';');
            if (variant == "ss") {
                libm_api_variant<float>(alibs, param, variant, test_mode, vendor, uth);
            } else if (variant == "vrs4") {
                libm_api_variant<libm::AlignedM128>(alibs, param, variant, test_mode, vendor, uth);
            } else if (variant == "vrs8") {
                libm_api_variant<libm::AlignedM256>(alibs, param, variant, test_mode, vendor, uth);
            } else if (variant == "vrs16") {
            #ifdef __AVX512F__
                libm_api_variant<libm::AlignedM512>(alibs, param, variant, test_mode, vendor, uth);
            #else
                std::cout << "AVX-512 not supported, skipping variant: " << variant << std::endl;
            #endif
            } else if (variant == "vrsa") {
                libm_api_variant<float>(alibs, param, variant, test_mode, vendor, uth);
            } else if (variant == "sd") {
                libm_api_variant<double>(alibs, param, variant, test_mode, vendor, uth);
            } else if (variant == "vrd2") {
                libm_api_variant<libm::AlignedM128d>(alibs, param, variant, test_mode, vendor, uth);
            } else if (variant == "vrd4") {
                libm_api_variant<libm::AlignedM256d>(alibs, param, variant, test_mode, vendor, uth);
            } else if (variant == "vrd8") {
            #ifdef __AVX512F__
                libm_api_variant<libm::AlignedM512d>(alibs, param, variant, test_mode, vendor, uth);
            #else
                std::cout << "AVX-512 not supported, skipping variant: " << variant << std::endl;
            #endif
            } else if (variant == "vrda") {
                libm_api_variant<double>(alibs, param, variant, test_mode, vendor, uth);
            } else if (variant == "sc") {
                libm_api_variant<fc32_t>(alibs, param, variant, test_mode, vendor, uth);
            } else if (variant == "sz") {
                libm_api_variant<fc64_t>(alibs, param, variant, test_mode, vendor, uth);
            } else {
                std::cout << "Invalid datatype: " << variant << std::endl;
            }
        }
        std::cout << std::endl;
    }
}

template void libm_api_variant<fc32_t>(struct AlmLibs *alibs, const struct YamlInputs &param,
                                       std::string &variant, TestMode test_mode,
                                       const std::string &vendor, std::string &ulp_threshold);
template void libm_api_variant<fc64_t>(struct AlmLibs *alibs, const struct YamlInputs &param,
                                       std::string &variant, TestMode test_mode,
                                       const std::string &vendor, std::string &ulp_threshold);