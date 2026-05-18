# AOCL-LibM Internal Utils Module

This module provides CPU detection functionality for AOCL-LibM, replacing the
external aocl-utils dependency with a self-contained, pure C implementation.

## API Compatibility

The module provides the same API as aocl-utils (`alci/arch.h`):

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

## Design Highlights

- **Single CPUID Query**: CPUID instructions are executed exactly once during
  initialization, minimizing runtime overhead
- **Cached Results**: All CPU information stored in a global structure for
  efficient repeated access
- **Efficient Feature Flags**: String-based flag requests converted to bitmask
  operations internally
- **Pure C**: No C++ runtime dependencies (no libstdc++)
- **Hidden Visibility**: Symbols are not exported to prevent conflicts (see below)

## Symbol Visibility and aocl-utils Coexistence

The internal utils use **hidden visibility** (`__attribute__((visibility("hidden")))`)
for all public API functions. This design choice addresses the concern of symbol
conflicts with external aocl-utils in Build-It-Yourself (BIY) scenarios.

### Why Hidden Visibility?

The API function names (`au_cpuid_arch_is_zen()`, `au_cpuid_has_flags()`, etc.)
are intentionally identical to external aocl-utils for API compatibility. However,
in scenarios where:

1. A user builds libm with internal utils
2. The same binary also links against external aocl-utils (e.g., for other AOCL
   libraries like AOCL-BLAS)

Symbol conflicts could occur at link time or runtime. Hidden visibility minimizes
this risk:

- **Shared libm (libalm.so)**: The internal utils symbols are not exported in
  the dynamic symbol table. libalm.so binds to its own internal hidden symbols,
  while other libraries or applications can use external aocl-utils separately
  without conflicts.
- **Static libm (libalm.a)**: When an application links libalm statically, the
  `au_cpuid_*` symbols become part of the final binary. If the application also
  links external aocl-utils (directly or via another AOCL library), symbol
  conflicts may occur despite hidden visibility (hidden visibility only affects
  shared library exports, not static linking).

**Note:** The internal utils module is built as a **static library only** (no
shared library variant). This is intentional to ensure the utils code is always
embedded directly into libm.

### Intended Use

The internal utils module is designed to **replace** external aocl-utils for libm,
not coexist with it. When building libm:

- Use internal utils (default) OR link against external aocl-utils
- Do not attempt to use both simultaneously within libm itself

### For Users

If you're building applications that link both:
- AOCL-LibM (with internal utils)
- Other AOCL libraries (using external aocl-utils)

The hidden visibility ensures no symbol conflicts. The application will use:
- Internal utils for libm's CPU detection (hidden, internal)
- External aocl-utils for other libraries (visible, exported)

## Building

### As part of AOCL-LibM (automatic)

The module is built automatically when building AOCL-LibM:

```bash
# CMake
cmake -B build && cmake --build build

# SCons
scons
```

### Standalone (for testing)

```bash
cd utils
cmake -B build && cmake --build build
ctest --test-dir build
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
