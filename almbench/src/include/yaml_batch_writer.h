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

#pragma once

#include <cstdint>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

#include "libm_yaml.h"
#include "ulp.h"

template <typename U>
struct YamlBatchWriter {
    struct Stats {
        ulp_data udata{};
        uint64_t total_tests  = 0;
        uint64_t fail_count   = 0;
        double   min_duration = std::numeric_limits<double>::max();
    };

    explicit YamlBatchWriter(const std::string &path, size_t flush_every = 512)
        : path_(path), flush_every_(flush_every)
    {
        buf_.reserve(flush_every);
    }

    void push(const struct YamlOutputs<U> *yop);
    Stats &stats() { return stats_; }
    const Stats &stats() const { return stats_; }
    void flush();
    ~YamlBatchWriter() { flush(); }

    bool emit_yaml_file = true;

private:
    std::string              path_;
    size_t                   flush_every_;
    std::vector<std::string> buf_;
    Stats                    stats_;
};
