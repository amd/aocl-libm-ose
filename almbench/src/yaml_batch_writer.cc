/*
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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

#include "yaml_batch_writer.h"

#include <iostream>
#include <limits>
#include <sstream>

#include <yaml-cpp/yaml.h>

#include "console_report.h"
#include "libm_yaml.h"

template <typename U>
void YamlBatchWriter<U>::push(const struct YamlOutputs<U> *yop)
{
    if (yop->config.test_mode == TestMode::E_PERFORMANCE) {
        if (yop->duration > 0.0 && yop->duration < stats_.min_duration) {
            stats_.min_duration = yop->duration;
        }
    }

    if (yop->status) {
        for (uint64_t j = 0; j < yop->n[0]; ++j) {
            stats_.total_tests++;
            if (yop->status[j] == TESTCASE_FAIL) {
                stats_.fail_count++;
            }
        }
    }

    if (yop->config.utflag &&
        yop->config.test_mode != TestMode::E_PERFORMANCE &&
        yop->status) {
        uint64_t fails = 0;
        for (uint64_t j = 0; j < yop->n[0]; ++j) {
            if (yop->status[j] == TESTCASE_FAIL) {
                fails++;
            }
        }
        report_accuracy_results<U>(yop, stats_.udata, yop->n[0], fails);
    }

    const bool verbose = is_verbose_mode_enabled();

    if (yop->config.utflag && !verbose) {
        emit_yaml_stdout<U>(yop);
    }

    if (emit_yaml_file && verbose) {
        YAML::Node node = serialize_yaml_outputs<U>(yop);
        std::ostringstream ss;
        ss << node;
        buf_.push_back(ss.str());
        if (buf_.size() >= flush_every_) {
            flush();
        }
    }
}

template <typename U>
void YamlBatchWriter<U>::flush()
{
    if (buf_.empty()) {
        return;
    }
    std::ofstream fout(path_, std::ios::app);
    if (!fout.is_open()) {
        std::cerr << "Error: Could not open file " << path_
                  << " for writing." << std::endl;
        buf_.clear();
        return;
    }
    for (auto &s : buf_) {
        fout << s << "\n";
    }
    buf_.clear();
}

template struct YamlBatchWriter<float>;
template struct YamlBatchWriter<double>;
template struct YamlBatchWriter<fc32_t>;
template struct YamlBatchWriter<fc64_t>;
