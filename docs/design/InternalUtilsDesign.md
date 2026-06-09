# Internal Utils Module Design

## Overview

This document describes the design of the internal CPU detection module (`utils/`) that replaces the external `aocl-utils` dependency in AOCL-LibM.

## Motivation

Previously, AOCL-LibM depended on an external library (`aocl-utils` / `libau_cpuid`) for runtime CPU detection. This had several drawbacks:

1. **External dependency**: Required users to build/install aocl-utils separately
2. **C++ runtime**: aocl-utils is a C++ library, requiring `libstdc++` linkage
3. **Build complexity**: Additional CMake/SCons configuration for finding and linking

The internal utils module addresses these issues by providing a self-contained, pure C implementation.

## Architecture

```
utils/
├── include/
│   └── alci/
│       └── arch.h          # Public API header (matches aocl-utils API)
├── src/
│   └── cpuid.c             # CPUID implementation
├── tests/
│   ├── CMakeLists.txt
│   └── test_cpuid.c        # Unit tests
├── CMakeLists.txt          # Standalone CMake build
├── SConscript              # SCons build integration
└── README.md               # Module documentation
```

## API Compatibility

The module provides the **exact same API** as aocl-utils (`alci/arch.h`):

```c
#include "alci/arch.h"

// Type definitions matching aocl-utils
typedef uint32_t au_cpu_num_t;

// Architecture detection
bool au_cpuid_arch_is_zen(au_cpu_num_t cpu_num);
bool au_cpuid_arch_is_zen2(au_cpu_num_t cpu_num);
bool au_cpuid_arch_is_zen3(au_cpu_num_t cpu_num);
bool au_cpuid_arch_is_zen4(au_cpu_num_t cpu_num);
bool au_cpuid_arch_is_zen5(au_cpu_num_t cpu_num);
bool au_cpuid_arch_is_zen6(au_cpu_num_t cpu_num);

// Feature flag detection  
bool au_cpuid_has_flags(au_cpu_num_t cpu_num, const char* const flags[], int count);

// CPU number constant
#define AU_CURRENT_CPU_NUM 0
```

This API compatibility means `src/iface.c` requires **no changes** to switch between internal and external implementations - only the include path and linked library change.

## Design Highlights

### 1. Single CPUID Query (Latency Optimization)

CPUID instructions are executed **exactly once** during program startup via constructor:

```c
static cpuid_info_t g_cpuid = {0};  // Global cached state

/* Runs before main() via constructor attribute */
INITIALIZER(cpuid_init)
{
    // Query CPUID leaves 0, 1, 7
    // ... populate g_cpuid ...
}
```

The `INITIALIZER` macro uses `__attribute__((constructor))` on GCC/Clang or `.CRT$XCU` section on MSVC to ensure initialization runs before `main()`. All subsequent calls (`au_cpuid_arch_is_zen4()`, `au_cpuid_has_flags()`, etc.) simply read from the cached global state.

### 2. Global Structure with Enums

CPU information is stored in a typed structure:

```c
typedef enum {
    UARCH_UNKNOWN = 0,
    UARCH_ZEN, UARCH_ZEN2, UARCH_ZEN3,
    UARCH_ZEN4, UARCH_ZEN5, UARCH_ZEN6
} uarch_t;

typedef enum {
    FEATURE_NONE      = 0,
    FEATURE_AVX512F   = (1 << 0),
    FEATURE_AVX512DQ  = (1 << 1)
} feature_t;

typedef struct {
    int          is_amd;
    uarch_t      uarch;
    unsigned int family;
    unsigned int model;
    unsigned int stepping;
    unsigned int features;  // Bitmask of feature_t
} cpuid_info_t;
```

### 3. Efficient Feature Flag Detection

Feature flags are read once and stored as a bitmask. Only features actually used by LibM are detected:

- **Leaf 7 (EBX)**: AVX512F, AVX512DQ (used by dispatcher for ISA selection)

```c
// During initialization - read flags at once
if ((leaf7_ebx >> AVX512F_BIT) & 1)  features |= FEATURE_AVX512F;
if ((leaf7_ebx >> AVX512DQ_BIT) & 1) features |= FEATURE_AVX512DQ;

// At runtime - simple bitmask check
bool au_cpuid_has_flags(au_cpu_num_t cpu_num, const char* const flags[], int count)
{
    unsigned int required = 0;
    for (int i = 0; i < count; i++)
        required |= flag_to_feature(flags[i]);
    
    return (g_cpuid.features & required) == required;
}
```

Supported feature flags (public API): `avx512f`, `avx512dq` (only flags used by LibM's iface.c).

### 4. AMD Zen Microarchitecture Detection

Detection based on CPUID Family/Model with feature-based fallback for unknown models:

| Family | Model | Microarchitecture | Notes |
|--------|-------|-------------------|-------|
| 0x17   | <= 0x1f | Zen / Zen+ | Explicit model-to-uarch mapping |
| 0x17   | >= 0x30 | Zen2 | Rome, Matisse, Renoir, etc. |
| 0x17   | 0x20-0x2f | Zen2 or Zen | Feature fallback: CLWB ? Zen2 : Zen |
| 0x19   | <= 0x0f | Zen3 | Milan, Chagall |
| 0x19   | 0x10-0x1f, 0x60-0xaf | Zen4 | Genoa, Raphael, Phoenix |
| 0x19   | Other models | Zen4 or Zen3 | Feature fallback: AVX512F ? Zen4 : Zen3 |
| 0x1A   | <= 0x4f, 0x60-0x77, 0xd0-0xd7 | Zen5 | Turin, Granite Ridge, Strix Point |
| 0x1A   | 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7 | Zen6 | Venice, future Zen6 products |
| 0x1A   | Other models | Zen5 | Unknown 0x1A models default to Zen5 |
| > 0x1A | Any | Zen6 | Future families default to Zen6 |
| Other  | Any | UNKNOWN | Non-AMD or unrecognized family |

#### Dispatcher Fallback Mechanism

When the CPUID module returns `UARCH_UNKNOWN` (non-AMD CPU or unrecognized family),
the dispatcher in `iface.c` handles the fallback:

1. Default to `ALM_UARCH_VER_DEFAULT` (AVX2 code path)
2. If CPU supports avx512f + avx512dq, upgrade to `ALM_UARCH_VER_AVX512`

Note: Unknown models within known AMD families (0x17, 0x19) use feature-based
detection within the CPUID module itself, not dispatcher fallback.

### 5. Pure C Implementation

- No C++ code or runtime dependency
- No `libstdc++` required
- Uses only standard C library (`<string.h>` for `strcmp`)
- Compiler-specific CPUID intrinsics abstracted via macros

### 6. Thread Safety

The global `g_cpuid` structure is designed for thread-safe access:

| Phase | Thread Safety | Notes |
|-------|---------------|-------|
| **After initialization** | ✅ Safe | Read-only access, no synchronization needed |
| **During initialization** | ⚠️ Benign race | Multiple threads may initialize concurrently |

**Why concurrent initialization is safe:**
- CPU information is immutable - same CPUID results regardless of which thread queries
- All threads write identical values (same CPU)
- Worst case: redundant CPUID queries (microseconds overhead)
- `initialized` flag is `volatile` to prevent compiler reordering

**Note:** In AOCL-LibM, applications do not call `au_cpuid_arch_is_*` directly.
The library's entry point function `init_map_entry_points()` is marked with
`__attribute__((constructor))`, which the dynamic loader guarantees to execute
exactly once during library load, even in multi-threaded applications. This
eliminates any concurrent initialization concerns.

## Build Integration

### CMake

```cmake
# Top-level CMakeLists.txt
add_subdirectory(utils)
set(AOCL_UTILS_INCLUDE_DIR ${ALM_UTILS_INCLUDE_DIR})

# src/CMakeLists.txt
set(EXTRA_LIBS alm_utils_static)  # Link internal utils
```

### SCons

```python
# src/SConscript
utils_objs, utils_inc = SConscript('#utils/SConscript', ...)
alm_objs += utils_objs
almenv.Append(CPPPATH=[utils_inc])
```

## Supported Platforms

- **Linux**: GCC, Clang (uses `<cpuid.h>`)
- **Windows**: Clang-CL (uses `<intrin.h>`) - Note: Pure MSVC is not supported

## Testing

Unit tests verify:
- Architecture detection (mutually exclusive)
- Feature flag detection (single and combined)
- Consistency of repeated calls (caching)

```bash
# Run tests (standalone build)
cd utils && cmake -B build && cmake --build build && ctest --test-dir build
```

## Migration from External aocl-utils

For users previously using external aocl-utils:

| Before | After |
|--------|-------|
| `--aocl_utils_install_path=<path>` | Not needed |
| `--aocl_utils_link=0/1` | Not needed |
| `-DAOCL_UTILS_INCLUDE_DIR=<path>` | Not needed |
| `-DAOCL_UTILS_LIB=<path>` | Not needed |
| Requires `libstdc++` | Pure C, no C++ runtime |

The build system now automatically uses the internal utils module with no additional configuration required.
