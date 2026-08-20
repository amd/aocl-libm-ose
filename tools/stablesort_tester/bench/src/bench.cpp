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
 * Standalone performance benchmark for the aocl-libm 64f indexed stable sort.
 *
 * Side A is always the built-in stablesort implementation (linked via
 * libalm). Side B is optional: an external shim shared library, loaded at
 * runtime via a platform dynamic loader (see aoclsort_shim.h). This keeps any
 * third-party sort
 * (e.g. IPP) entirely outside the aocl-libm build - you build the shim
 * separately and pass it with --shim.
 *
 * For each (size x distribution x stride) cell - or each .f64 input file - the
 * harness generates the input once, then times only the sort call across
 * --iterations runs, reporting min/median/mean ns and throughput. It also
 * verifies the result is a stable ascending permutation (reported as ok=0/1).
 */

#include <amdlibm.h>

#include "aoclsort_shim.h"
#include "dl_load.h"
#include "dist.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace {

namespace fs = std::filesystem;
using aoclsort_bench::Dist;
using clk = std::chrono::steady_clock;

// ---------------------------------------------------------------------------
// Command-line arguments.

struct Args {
  std::vector<std::size_t> sizes = {512, 102400, 1048576};
  std::vector<std::string> dists = {"uniform",       "sorted",   "reverse",
                                    "nearly_sorted", "few_unique", "zipf",
                                    "sparse_high"};
  std::vector<int> strides = {(int)sizeof(double)};
  std::vector<std::string> data; // .f64 files or directories
  std::string shim_path;         // optional side-B shim .so
  std::string dump_path;         // optional CSV output
  int iterations = 11;
  std::uint64_t seed = 12345;
};

void usage() {
  std::fprintf(
      stderr,
      "Usage: stablesort_bench [options]\n"
      "  --sizes <list>       Comma-separated element counts (K/M/G suffix "
      "OK). Default: 512,100K,1M\n"
      "  --dists <list>       Comma-separated distributions: uniform,sorted,"
      "reverse,nearly_sorted,few_unique,zipf,sparse_high\n"
      "  --strides <list>     Comma-separated src strides in BYTES (>= 8). "
      "Default: 8 (dense)\n"
      "  --data <paths>       Comma-separated .f64 files or directories; "
      "benchmarks real data instead of generated input\n"
      "  --shim <path>        External side-B shim (.so/.dll) to A/B compare "
      "against (loaded at runtime; see aoclsort_shim.h)\n"
      "  --dump <path>        Append per-cell results to a CSV file\n"
      "  --iterations <n>     Timed iterations per cell. Default: 11\n"
      "  --seed <n>           RNG seed for generated input. Default: 12345\n"
      "  -h, --help           Show this help\n");
}

std::vector<std::string> split(const std::string &s, char sep) {
  std::vector<std::string> out;
  std::size_t i = 0;
  while (i <= s.size()) {
    std::size_t j = s.find(sep, i);
    if (j == std::string::npos) {
      j = s.size();
    }
    out.push_back(s.substr(i, j - i));
    i = j + 1;
  }
  return out;
}

std::size_t parse_count(const std::string &s) {
  if (s.empty()) {
    return 0;
  }
  std::size_t mult = 1;
  char suf = s.back();
  std::size_t n = s.size();
  if (suf == 'K' || suf == 'k') {
    mult = std::size_t{1} << 10;
    --n;
  } else if (suf == 'M' || suf == 'm') {
    mult = std::size_t{1} << 20;
    --n;
  } else if (suf == 'G' || suf == 'g') {
    mult = std::size_t{1} << 30;
    --n;
  }
  return mult * std::strtoull(s.substr(0, n).c_str(), nullptr, 10);
}

// Returns false on a parse error (message already printed).
bool parse_args(int argc, char **argv, Args &a) {
  for (int i = 1; i < argc; ++i) {
    std::string k = argv[i];
    auto need = [&](void) -> std::string {
      if (i + 1 >= argc) {
        std::fprintf(stderr, "missing value for %s\n", k.c_str());
        usage();
        std::exit(2);
      }
      return std::string(argv[++i]);
    };
    if (k == "--sizes") {
      a.sizes.clear();
      for (auto &s : split(need(), ',')) {
        a.sizes.push_back(parse_count(s));
      }
    } else if (k == "--dists") {
      a.dists = split(need(), ',');
    } else if (k == "--strides") {
      a.strides.clear();
      for (auto &s : split(need(), ',')) {
        a.strides.push_back((int)std::strtol(s.c_str(), nullptr, 10));
      }
    } else if (k == "--data") {
      a.data = split(need(), ',');
    } else if (k == "--shim") {
      a.shim_path = need();
    } else if (k == "--dump") {
      a.dump_path = need();
    } else if (k == "--iterations") {
      a.iterations = (int)std::strtol(need().c_str(), nullptr, 10);
    } else if (k == "--seed") {
      a.seed = std::strtoull(need().c_str(), nullptr, 10);
    } else if (k == "-h" || k == "--help") {
      usage();
      std::exit(0);
    } else {
      std::fprintf(stderr, "unknown argument: %s\n", k.c_str());
      usage();
      return false;
    }
  }
  if (a.iterations < 1) {
    std::fprintf(stderr, "--iterations must be >= 1 (got %d)\n", a.iterations);
    usage();
    return false;
  }
  return true;
}

// ---------------------------------------------------------------------------
// Sort implementation handle (side A built-in, or side B dynamically loaded shim).

struct SortApi {
  std::string name;
  int (*get_size)(int, int *);
  int (*sort)(const double *, int, int *, int, void *);
  DL_HANDLE handle; // shared-library handle for a shim; nullptr for the built-in.
};

// Thin forwarders so the built-in is invoked through exactly the same one-extra
// -call indirection as an external shim, keeping the A/B comparison symmetric.
int builtin_get_size(int len, int *workspace_size) {
  return amd_stablesort_getsize_64f(len, workspace_size);
}

int builtin_sort(const double *src, int src_stride_bytes, int *dst_index,
                 int len, void *workspace) {
  return amd_stablesort_ascend_64f(src, src_stride_bytes, dst_index, len,
                                   workspace);
}

SortApi make_builtin() {
  return {"amdsort", &builtin_get_size, &builtin_sort, nullptr};
}

// Returns false on failure (message already printed).
bool load_shim(const std::string &path, SortApi &out) {
  DL_HANDLE h = DL_LOAD(path.c_str());
  if (h == nullptr) {
    std::fprintf(stderr, "failed to load shim '%s': %s\n", path.c_str(),
                 DL_ERROR());
    return false;
  }
  auto gs =
      (int (*)(int, int *))DL_SYM(h, "aoclsort_shim_get_size_64f");
  auto st = (int (*)(const double *, int, int *, int, void *))DL_SYM(
      h, "aoclsort_shim_sort_indexed_ascend_64f");
  if (gs == nullptr || st == nullptr) {
    std::fprintf(stderr,
                 "shim '%s' is missing required symbols "
                 "(aoclsort_shim_get_size_64f / "
                 "aoclsort_shim_sort_indexed_ascend_64f)\n",
                 path.c_str());
    DL_CLOSE(h);
    return false;
  }
  auto nm = (const char *(*)())DL_SYM(h, "aoclsort_shim_name");
  std::string name =
      nm != nullptr ? std::string(nm()) : fs::path(path).stem().string();
  out = {name, gs, st, h};
  return true;
}

// ---------------------------------------------------------------------------
// Input preparation and verification.

// Scatters dense doubles into a buffer with `stride` bytes between elements.
std::vector<std::uint8_t> make_strided(const std::vector<double> &dense,
                                       int stride) {
  std::size_t n = dense.size();
  std::vector<std::uint8_t> buf(n * (std::size_t)stride);
  for (std::size_t i = 0; i < n; ++i) {
    std::memcpy(buf.data() + i * (std::size_t)stride, &dense[i],
                sizeof(double));
  }
  return buf;
}

double key_at(const std::uint8_t *base, int stride, int i) {
  double v;
  std::memcpy(&v, base + (std::size_t)i * (std::size_t)stride, sizeof(double));
  return v;
}

// Matches alm_sort_to_sortable() in stablesort.c (IEEE-754 total order).
std::uint64_t sortable_key(double d) {
  std::uint64_t u;
  std::memcpy(&u, &d, sizeof(u));
  std::int64_t s = -(std::int64_t)(u >> 63);
  return u ^ ((std::uint64_t)s | 0x8000000000000000ULL);
}

// Checks idx is a permutation of [0, n) and that keys read through it are
// non-decreasing in the library's total order with stability (equal keys keep
// ascending original index).
bool verify(const std::uint8_t *base, int stride, const int *idx,
            std::size_t n) {
  std::vector<char> seen(n, 0);
  for (std::size_t i = 0; i < n; ++i) {
    int v = idx[i];
    if (v < 0 || (std::size_t)v >= n || seen[(std::size_t)v]) {
      return false;
    }
    seen[(std::size_t)v] = 1;
  }
  for (std::size_t i = 1; i < n; ++i) {
    std::uint64_t a = sortable_key(key_at(base, stride, idx[i - 1]));
    std::uint64_t b = sortable_key(key_at(base, stride, idx[i]));
    if (a > b) {
      return false;
    }
    if (a == b && idx[i - 1] > idx[i]) {
      return false;
    }
  }
  return true;
}

// ---------------------------------------------------------------------------
// Measurement.

struct Result {
  long long min_ns = 0;
  long long med_ns = 0;
  long long mean_ns = 0;
  double melem_s = 0.0;
  bool ok = false;
  int status = 0;
};

long long ns_since(clk::time_point t0, clk::time_point t1) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
}

Result run_cell(const SortApi &api, const std::uint8_t *keys, int stride,
                std::size_t n, int iters) {
  Result r;
  int ws_bytes = 0;
  r.status = api.get_size((int)n, &ws_bytes);
  if (r.status != 0) {
    return r;
  }
  std::vector<std::uint8_t> ws(ws_bytes > 0 ? (std::size_t)ws_bytes : 0);
  std::vector<int> idx(n);
  std::vector<long long> times((std::size_t)iters);
  int last = 0;
  for (int it = 0; it < iters; ++it) {
    auto t0 = clk::now();
    last = api.sort((const double *)keys, stride, idx.data(), (int)n,
                    ws_bytes > 0 ? ws.data() : nullptr);
    auto t1 = clk::now();
    times[(std::size_t)it] = ns_since(t0, t1);
  }
  r.status = last;
  std::sort(times.begin(), times.end());
  r.min_ns = times.front();
  r.med_ns = times[(std::size_t)iters / 2];
  long long sum = 0;
  for (auto x : times) {
    sum += x;
  }
  r.mean_ns = sum / iters;
  r.ok = (last == 0) && verify(keys, stride, idx.data(), n);
  r.melem_s = r.min_ns > 0 ? (double)n * 1000.0 / (double)r.min_ns : 0.0;
  return r;
}

// ---------------------------------------------------------------------------
// Output.

void print_header() {
  std::printf("%-10s %-14s %12s %7s %6s %12s %12s %12s %10s %3s\n", "impl",
              "dist", "n", "stride", "iters", "min_ns", "med_ns", "mean_ns",
              "MElem/s", "ok");
}

void print_row(const std::string &impl, const std::string &dist,
               std::size_t n, int stride, int iters, const Result &r) {
  std::printf("%-10s %-14s %12zu %7d %6d %12lld %12lld %12lld %10.1f %3d\n",
              impl.c_str(), dist.c_str(), n, stride, iters, r.min_ns, r.med_ns,
              r.mean_ns, r.melem_s, r.ok ? 1 : 0);
}

void dump_row(std::ofstream &os, const std::string &impl,
              const std::string &dist, std::size_t n, int stride, int iters,
              const Result &r) {
  os << impl << ',' << dist << ',' << n << ',' << stride << ',' << iters << ','
     << r.min_ns << ',' << r.med_ns << ',' << r.mean_ns << ',' << r.melem_s
     << ',' << (r.ok ? 1 : 0) << '\n';
}

// ---------------------------------------------------------------------------
// Data file loading.

// Loads a raw little-endian binary64 (.f64) file into `out`. Returns false
// (with a message) on missing file, empty file, or size not a multiple of 8.
bool load_f64(const fs::path &path, std::vector<double> &out) {
  std::error_code ec;
  if (!fs::is_regular_file(path, ec)) {
    std::fprintf(stderr, "data file not found: %s\n", path.string().c_str());
    return false;
  }
  std::uintmax_t bytes = fs::file_size(path, ec);
  if (ec || bytes == 0) {
    std::fprintf(stderr, "data file empty or unreadable: %s\n",
                 path.string().c_str());
    return false;
  }
  if (bytes % sizeof(double) != 0) {
    std::fprintf(stderr,
                 "data file size (%llu bytes) is not a multiple of 8, not a "
                 "valid .f64 file: %s\n",
                 (unsigned long long)bytes, path.string().c_str());
    return false;
  }
  std::size_t count = (std::size_t)(bytes / sizeof(double));
  if (count > (std::size_t)INT32_MAX) {
    std::fprintf(stderr,
                 "data file too large: %zu elements exceeds the sort's int "
                 "length limit (%d), skipping: %s\n",
                 count, INT32_MAX, path.string().c_str());
    return false;
  }
  out.resize(count);
  std::ifstream in(path, std::ios::binary);
  if (!in.read((char *)out.data(), (std::streamsize)bytes)) {
    std::fprintf(stderr, "failed to read data file: %s\n",
                 path.string().c_str());
    return false;
  }
  return true;
}

// Expands --data entries (files and directories) into a list of .f64 files.
std::vector<fs::path> collect_data_files(const std::vector<std::string> &data) {
  std::vector<fs::path> files;
  for (const auto &entry : data) {
    fs::path p(entry);
    std::error_code ec;
    if (fs::is_directory(p, ec)) {
      bool any = false;
      for (const auto &de : fs::directory_iterator(p, ec)) {
        if (de.is_regular_file() && de.path().extension() == ".f64") {
          files.push_back(de.path());
          any = true;
        }
      }
      if (!any) {
        std::fprintf(stderr, "no .f64 files found in directory: %s\n",
                     entry.c_str());
      }
    } else if (fs::exists(p, ec)) {
      if (p.extension() != ".f64") {
        std::fprintf(stderr, "skipping non-.f64 file: %s\n", entry.c_str());
      } else {
        files.push_back(p);
      }
    } else {
      std::fprintf(stderr, "data path does not exist: %s\n", entry.c_str());
    }
  }
  std::sort(files.begin(), files.end());
  return files;
}

// ---------------------------------------------------------------------------

// Benchmarks a single cell (one distribution or one data file).
void process_cell(const Args &a, const std::vector<SortApi> &apis,
                  const std::string &label, const std::vector<double> &dense,
                  std::ofstream *dump) {
  std::size_t n = dense.size();
  if (n == 0) {
    return;
  }
  if (n > (std::size_t)INT32_MAX) {
    std::fprintf(stderr, "skipping %s: n=%zu exceeds int range\n",
                 label.c_str(), n);
    return;
  }
  for (int stride : a.strides) {
    if (stride < (int)sizeof(double)) {
      std::fprintf(stderr, "skipping stride %d (< 8 bytes)\n", stride);
      continue;
    }
    std::vector<std::uint8_t> buf = make_strided(dense, stride);
    for (const auto &api : apis) {
      Result r = run_cell(api, buf.data(), stride, n, a.iterations);
      print_row(api.name, label, n, stride, a.iterations, r);
      if (dump != nullptr) {
        dump_row(*dump, api.name, label, n, stride, a.iterations, r);
      }
    }
  }
}

} // namespace

int main(int argc, char **argv) {
  Args a;
  if (!parse_args(argc, argv, a)) {
    return 2;
  }

  std::vector<SortApi> apis;
  apis.push_back(make_builtin());
  if (!a.shim_path.empty()) {
    SortApi shim;
    if (!load_shim(a.shim_path, shim)) {
      return 1;
    }
    apis.push_back(shim);
  }

  // Generator mode
  std::vector<std::pair<std::string, Dist>> gen;
  if (a.data.empty()) {
    for (const auto &dname : a.dists) {
      Dist d;
      if (!aoclsort_bench::dist_parse(dname, d)) {
        std::fprintf(stderr, "unknown distribution: %s\n", dname.c_str());
        return 2;
      }
      gen.emplace_back(dname, d);
    }
  }

  std::ofstream dump;
  std::ofstream *dump_ptr = nullptr;
  if (!a.dump_path.empty()) {
    bool write_header = !fs::exists(a.dump_path) ||
                        fs::file_size(a.dump_path) == 0;
    dump.open(a.dump_path, std::ios::app);
    if (!dump) {
      std::fprintf(stderr, "failed to open dump file: %s\n",
                   a.dump_path.c_str());
      return 1;
    }
    if (write_header) {
      dump << "impl,dist,n,stride,iters,min_ns,med_ns,mean_ns,melem_s,ok\n";
    }
    dump_ptr = &dump;
  }

  print_header();

  // Process one cell at a time so only a single cell's keys are resident in
  // memory: either each real .f64 file, or each generated distribution.
  if (!a.data.empty()) {
    bool processed = false;
    for (const auto &path : collect_data_files(a.data)) {
      std::vector<double> dense;
      if (load_f64(path, dense)) {
        process_cell(a, apis, path.stem().string(), dense, dump_ptr);
        processed = true;
      }
    }
    if (!processed) {
      std::fprintf(stderr, "no valid .f64 data to benchmark\n");
      return 1;
    }
  } else {
    for (std::size_t n : a.sizes) {
      if (n > (std::size_t)INT32_MAX) {
        std::fprintf(stderr, "skipping size %zu: exceeds int range\n", n);
        continue;
      }
      for (const auto &g : gen) {
        std::vector<double> dense(n);
        aoclsort_bench::fill_dist(dense.data(), n, g.second, a.seed);
        process_cell(a, apis, g.first, dense, dump_ptr);
      }
    }
  }

  for (auto &api : apis) {
    if (api.handle != nullptr) {
      DL_CLOSE(api.handle);
    }
  }
  return 0;
}
