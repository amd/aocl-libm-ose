# stablesort tester

Standalone test/benchmark tooling for the aocl-libm **stablesort** API. This
directory is deliberately isolated from the rest of the aocl-libm test
infrastructure: the stablesort API is not exercised by `almbench` or the main
`gtests`, so everything specific to it lives here.

The public API (declared in `amdlibm.h`) is a stable, indirect
(index-producing) ascending sort of double-precision keys:

```c
int amd_stablesort_getsize_64f(int len, int *workspace_size);
int amd_stablesort_ascend_64f(const double *src, int src_stride_bytes,
                              int *dst_index, int len, void *workspace);
```

`amd_stablesort_getsize_64f` reports the scratch buffer size;
`amd_stablesort_ascend_64f` writes a stable sorted permutation of `[0, len)`
into `dst_index`.

## Building

The tester is off by default. Enable it with the `SORT_BUILD_TESTS` CMake
option (it builds against the aocl-libm shared library, `libalm.so`):

```bash
cmake -S . -B <build-dir> -DSORT_BUILD_TESTS=ON
cmake --build <build-dir> --target stablesort_bench
```

On Linux, point the loader at the built library:

```bash
LD_LIBRARY_PATH=<build-dir>/src ./<build-dir>/tools/stablesort_tester/bench/stablesort_bench --help
```

On Windows, ensure `libalm.dll` is on `PATH` (or in the same directory as the
benchmark executable) before running `stablesort_bench.exe`.

## Benchmark (`stablesort_bench`)

`stablesort_bench` measures the built-in stablesort 64f indexed sort (side A)
over a grid of sizes, distributions and strides, timing only the sort call. It
also verifies each result (see the `ok` column under [Output](#output)).

| Option | Meaning |
| --- | --- |
| `--sizes <list>` | Element counts, K/M/G suffixes allowed (default `512,100K,1M`). |
| `--dists <list>` | Distributions: `uniform,sorted,reverse,nearly_sorted,few_unique,zipf,sparse_high`. |
| `--strides <list>` | `src_stride_bytes` values, `>= 8` (default `8`, i.e. dense). |
| `--data <paths>` | Benchmark real `.f64` files/directories instead of generated input. |
| `--shim <path>` | A/B-compare against an external shim (`.so` on Linux, `.dll` on Windows; see below). |
| `--dump <path>` | Append per-cell results to a CSV. |
| `--iterations <n>` | Timed iterations per cell (default `11`). |
| `--seed <n>` | RNG seed for generated input (default `12345`). |

Example: generated data, two distributions, dense and strided:

```bash
LD_LIBRARY_PATH=<build-dir>/src ./<build-dir>/tools/stablesort_tester/bench/stablesort_bench \
    --sizes 256K --dists uniform,zipf --strides 8,64 --iterations 11 \
    --dump results.csv
```

Real `.f64` data (raw little-endian binary64, element count = `filesize / 8`
auto-derived; empty files, non-multiple-of-8 sizes and missing paths are
reported and skipped):

```bash
LD_LIBRARY_PATH=<build-dir>/src ./<build-dir>/tools/stablesort_tester/bench/stablesort_bench --data /path/to/data_dir
```

### Output

Results are printed to stdout as a table. `--dump <path>` writes the same 
rows to a CSV:

| Column | Meaning |
| --- | --- |
| `impl` | Implementation measured: `amdsort` (built-in) or the shim's name. |
| `dist` | Distribution name, or the `.f64` file stem in `--data` mode. |
| `n` | Element count for the cell. |
| `stride` | `src_stride_bytes` used for this cell. |
| `iters` | Timed iterations. |
| `min_ns` / `med_ns` / `mean_ns` | Min / median / mean sort time in nanoseconds over the iterations. |
| `MElem/s` | Throughput (millions of elements per second) from `min_ns`. |
| `ok` | `1` if the result is a correct stable ascending permutation of `[0, n)`, else `0`. |

### A/B comparison via a shim

Side B is any other sort you want to compare against (e.g. Intel IPP). The
benchmark doesn't bundle it: you build a tiny shared library that implements the
shim ABI, and the benchmark loads it at runtime via `--shim` (dlopen on Linux,
LoadLibrary on Windows).

The shim should export two required C functions (and one optional label), mirroring
the public stablesort API. Full contract is in
[`bench/include/aoclsort_shim.h`](bench/include/aoclsort_shim.h):

```cpp
// my_shim.cpp — wrap your sort of choice behind the shim ABI.
#include <cstddef>

extern "C" {

// Report the scratch size your sort needs for `len` elements.
int aoclsort_shim_get_size_64f(int len, int *workspace_size) {
  *workspace_size = /* bytes your sort needs (0 if none) */ 0;
  return 0;                       // 0 = ok, negative = invalid input
}

// Write a stable ascending permutation of [0, len) into dst_index, reading
// element i of src at byte offset i * src_stride_bytes.
int aoclsort_shim_sort_indexed_ascend_64f(const double *src, int src_stride_bytes,
                                          int *dst_index, int len, void *workspace) {
  // ... call your sort here ...
  return 0;
}

// Optional: short label shown in the "impl" column (defaults to the .so name).
const char *aoclsort_shim_name(void) { return "mysort"; }

} // extern "C"
```

Build it against the shim header and pass it with `--shim`:

```bash
# Linux
g++ -O2 -shared -fPIC -I tools/stablesort_tester/bench/include \
    my_shim.cpp -o libmy_shim.so

LD_LIBRARY_PATH=<build-dir>/src ./<build-dir>/tools/stablesort_tester/bench/stablesort_bench \
    --sizes 1M --dists uniform --shim ./libmy_shim.so
```

```bat
REM Windows (MSVC)
cl /LD /O2 /I tools\stablesort_tester\bench\include my_shim.cpp /Fe:my_shim.dll

set PATH=<build-dir>\src;%PATH%
<build-dir>\tools\stablesort_tester\bench\stablesort_bench.exe ^
    --sizes 1M --dists uniform --shim my_shim.dll
```

The benchmark then prints one row per side for each cell. The `ok` column
reports whether each side produced a correct stable ascending permutation, so
A/B runs double as a cross-check.

## Tests (`stablesort_gtest`)

Unit/API tests (gtest-based, reusing the vendored framework under `gtests/gapi`).

```bash
cmake -S . -B <build-dir> -DSORT_BUILD_TESTS=ON
cmake --build <build-dir> --target stablesort_gtest
./<build-dir>/tools/stablesort_tester/gtests/stablesort_gtest
```

Optional gcov coverage report (GCC, non-Windows only):

```bash
cmake ... -DSTABLESORT_TESTS_COVERAGE=ON -DSTABLESORT_TESTS_ASAN=OFF
cmake --build <build-dir> --target stablesort_coverage
```
