# AOCL-LibM Internal Utils Module

This module provides CPU detection functionality for AOCL-LibM, replacing the
external aocl-utils dependency with a self-contained, pure C implementation.

## API

The module provides CPU detection via `alm_arch.h`:

```c
#include "alm_arch.h"

// Type definition
typedef uint32_t alm_cpu_num_t;

// Architecture detection (uses "at-least" semantics)
bool alm_cpuid_arch_is_zen(alm_cpu_num_t cpu_num);
bool alm_cpuid_arch_is_zen2(alm_cpu_num_t cpu_num);
bool alm_cpuid_arch_is_zen3(alm_cpu_num_t cpu_num);
bool alm_cpuid_arch_is_zen4(alm_cpu_num_t cpu_num);
bool alm_cpuid_arch_is_zen5(alm_cpu_num_t cpu_num);
bool alm_cpuid_arch_is_zen6(alm_cpu_num_t cpu_num);

// Feature flag detection
bool alm_cpuid_has_flags(alm_cpu_num_t cpu_num, const char* const flags[], int count);

// CPU number constant
#define ALM_CURRENT_CPU_NUM 0
```

## Design Highlights

- **Unique Symbol Prefix**: All types and macros use `ALM_`/`alm_` prefix to avoid
  conflicts with external aocl-utils in BIY scenarios
- **Static Inline Functions**: API functions are `static inline` in the header,
  eliminating symbol exports entirely from the object file
- **Single CPUID Query**: CPUID instructions are executed exactly once during
  initialization, minimizing runtime overhead
- **Cached Results**: All CPU information stored in global variables for
  efficient repeated access (only two symbols exported: `alm_g_detected_uarch`
  and `alm_g_detected_features`)
- **Efficient Feature Flags**: String-based flag requests converted to bitmask
  operations internally
- **Pure C**: No C++ runtime dependencies (no libstdc++)

## BIY (Build-It-Yourself) Compatibility

When building a unified AOCL library that combines multiple AOCL components
(e.g., `libaocl.so` containing aocl-libm, aocl-utils, aocl-blas, etc.):

- **No symbol conflicts**: The `alm_cpuid_*` prefix is different from the
  external aocl-utils `au_cpuid_*` prefix, so both can coexist
- **Independent operation**: Each library uses its own CPU detection:
  - aocl-libm uses `alm_cpuid_*` (internal utils)
  - aocl-utils provides `au_cpuid_*` (for other AOCL libraries)

This eliminates the "multiple definition" linker errors that would occur if
both libraries used the same symbol names.

## Building

### As part of AOCL-LibM (automatic)

The CPU-detection source is built as a dedicated object library
(`alm_utils_obj`) under `src/utils` and embedded directly into libalm (both the
static and shared libraries), so no separate utils archive is produced:

```bash
# CMake
cmake -B build && cmake --build build

# SCons
scons
```

### Running the unit tests

The tests are gated on the `UTILS_BUILD_TESTS` option (distinct from
`LIBM_BUILD_TESTS`, which controls the gtests suite). Tests that exercise the
public detection API link the self-contained libalm; the branch-coverage test
is standalone.

```bash
cmake --preset dev-release-gcc -DUTILS_BUILD_TESTS=ON
cmake --build --preset dev-release-gcc --target cpuid_tests_all
ctest --test-dir build/dev-release-gcc/cpuid_tests
```

## Supported Features

- AMD Zen through Zen6 microarchitecture detection
- AVX-512 feature detection (avx512f, avx512dq - only flags used by LibM dispatcher)
- Unknown AMD model handling:
  - Family 0x17, model <= 0x1f: Zen
  - Family 0x17, model 0x20-0x2f: CLWB flag ? Zen2 : Zen (feature fallback)
  - Family 0x17, model >= 0x30: Zen2
  - Family 0x19, model <= 0x0f: Zen3
  - Family 0x19, model 0x10-0x1f or 0x60-0xaf: Zen4
  - Family 0x19, other models: AVX512F flag ? Zen4 : Zen3 (feature fallback)
  - Family 0x1A, Zen5 models: <= 0x4f, 0x60-0x77, 0xd0-0xd7
  - Family 0x1A, Zen6 models: 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7
  - Family 0x1A, unknown models: Zen5 (default)
  - Family > 0x1A: Zen6 (forward compatibility)
- Non-AMD CPUs return UARCH_UNKNOWN; dispatcher in `iface.c` falls back to AVX2/AVX512 based on features

## Supported Platforms

- **Linux**: GCC, Clang
- **Windows**: Clang-CL (pure MSVC not supported)
