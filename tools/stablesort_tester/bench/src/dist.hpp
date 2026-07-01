/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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

/*
 * Input distribution generators for the stablesort benchmark.
 *
 * Specialised to double precision (the type of the 64f API)
 * Distributions stress different parts of a radix/merge sort:
 *
 *   uniform        random over a wide range; the standard baseline.
 *   sorted         sorted already ascending.
 *   reverse        sorted but descending.
 *   nearly_sorted  sorted + ~1% random swaps.
 *   few_unique     only 16 distinct values, scattered.
 *   zipf           a few values are very common and a long tail of values are rare.
 *   sparse_high    only the low byte varies (small int-valued doubles).
 *
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>

namespace aoclsort_bench {

enum class Dist {
  Uniform,
  Sorted,
  Reverse,
  NearlySorted,
  FewUnique,
  Zipf,
  SparseHigh,
};

inline const char *dist_name(Dist d) {
  switch (d) {
  case Dist::Uniform:
    return "uniform";
  case Dist::Sorted:
    return "sorted";
  case Dist::Reverse:
    return "reverse";
  case Dist::NearlySorted:
    return "nearly_sorted";
  case Dist::FewUnique:
    return "few_unique";
  case Dist::Zipf:
    return "zipf";
  case Dist::SparseHigh:
    return "sparse_high";
  }
  return "?";
}

// Returns true and sets `out` on a recognised name; false otherwise.
inline bool dist_parse(const std::string &s, Dist &out) {
  if (s == "uniform") {
    out = Dist::Uniform;
  } else if (s == "sorted") {
    out = Dist::Sorted;
  } else if (s == "reverse") {
    out = Dist::Reverse;
  } else if (s == "nearly_sorted") {
    out = Dist::NearlySorted;
  } else if (s == "few_unique") {
    out = Dist::FewUnique;
  } else if (s == "zipf") {
    out = Dist::Zipf;
  } else if (s == "sparse_high") {
    out = Dist::SparseHigh;
  } else {
    return false;
  }
  return true;
}

inline void fill_uniform(double *p, std::size_t n, std::mt19937_64 &rng) {
  std::uniform_real_distribution<double> d(-1e6, 1e6);
  for (std::size_t i = 0; i < n; ++i) {
    p[i] = d(rng);
  }
}

inline void fill_sorted(double *p, std::size_t n) {
  for (std::size_t i = 0; i < n; ++i) {
    p[i] = static_cast<double>(i);
  }
}

inline void fill_reverse(double *p, std::size_t n) {
  for (std::size_t i = 0; i < n; ++i) {
    p[i] = static_cast<double>(n - 1 - i);
  }
}

inline void fill_nearly_sorted(double *p, std::size_t n, std::mt19937_64 &rng) {
  fill_sorted(p, n);
  std::size_t swaps = std::max<std::size_t>(1, n / 100); // 1% of the array
  std::uniform_int_distribution<std::size_t> d(0, n ? n - 1 : 0);
  for (std::size_t i = 0; i < swaps; ++i) {
    std::swap(p[d(rng)], p[d(rng)]);
  }
}

inline void fill_few_unique(double *p, std::size_t n, std::mt19937_64 &rng) {
  constexpr int K = 16;
  double vals[K];
  fill_uniform(vals, K, rng);
  std::uniform_int_distribution<int> d(0, K - 1);
  for (std::size_t i = 0; i < n; ++i) {
    p[i] = vals[d(rng)];
  }
}

// Inverse-CDF approximation for Zipf(s~1): rank = floor(N^U), U~Unif[0,1].
// Small values dominate with a long thin tail of large values
inline void fill_zipf(double *p, std::size_t n, std::mt19937_64 &rng) {
  std::uniform_real_distribution<double> d(0.0, 1.0);
  double lnN = std::log(static_cast<double>(std::max<std::size_t>(2, n)));
  for (std::size_t i = 0; i < n; ++i) {
    p[i] = std::exp(d(rng) * lnN);
  }
}

// Only the low byte (0..255) varies; high bytes ~0.
inline void fill_sparse_high(double *p, std::size_t n, std::mt19937_64 &rng) {
  std::uniform_int_distribution<int> d(0, 255);
  for (std::size_t i = 0; i < n; ++i) {
    p[i] = static_cast<double>(d(rng));
  }
}

inline void fill_dist(double *p, std::size_t n, Dist d, std::uint64_t seed) {
  std::mt19937_64 rng(seed);
  switch (d) {
  case Dist::Uniform:
    fill_uniform(p, n, rng);
    break;
  case Dist::Sorted:
    fill_sorted(p, n);
    break;
  case Dist::Reverse:
    fill_reverse(p, n);
    break;
  case Dist::NearlySorted:
    fill_nearly_sorted(p, n, rng);
    break;
  case Dist::FewUnique:
    fill_few_unique(p, n, rng);
    break;
  case Dist::Zipf:
    fill_zipf(p, n, rng);
    break;
  case Dist::SparseHigh:
    fill_sparse_high(p, n, rng);
    break;
  }
}

} // namespace aoclsort_bench
