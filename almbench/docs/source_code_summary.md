# AMD LibM Test Suite - Source Code Analysis Summary

**Generated:** October 10, 2025
**Location:** `almbench/src/` and `almbench/src/include/`

---

## Overview

The AMD LibM test suite is a comprehensive testing framework for validating math library functions across multiple API variants (scalar, SIMD vector, and VRA - Vector Real Array). It supports both accuracy verification using ULP (Unit in Last Place) calculations and performance benchmarking.

---

## Core Architecture

### Design Patterns
- **Template-based polymorphism**: Supports multiple data types (float, double) and SIMD widths (128/256/512-bit)
- **Strategy pattern**: Different generators for input range creation
- **Factory pattern**: API prototype dispatchers based on function signature
- **YAML-driven testing**: Configuration and results serialization

### Test Modes
1. **Unit Test Mode** (`E_UNITTEST`): Single input/output validation
2. **Accuracy Mode** (`E_ACCURACY`): Range-based ULP error verification
3. **Performance Mode** (`E_PERFORMANCE`): Timing measurements with multiple iterations
4. **Known Test Mode** (`E_KNOWNTEST`): Pre-defined test cases

---

## Header Files (`src/include/`)

### 1. **alm_test.h** - Core Type Definitions
**Purpose**: Foundation header defining all core data structures and type traits

**Key Components**:
- **SIMD Wrapper Types**:
  - `AlignedM128`, `AlignedM128d` (SSE, 128-bit)
  - `AlignedM256`, `AlignedM256d` (AVX, 256-bit)
  - `AlignedM512`, `AlignedM512d` (AVX-512, 512-bit, conditional)

- **Template Trait System** (`type_info<T>`):
  - Maps SIMD types to scalar equivalents
  - Provides `is_simd` flag and `real_type` alias

- **Core Structures**:
  - `AlmLibs`: Holds dynamic library handles (shim + reference)
  - `InParams<T,U>`: Input parameters with support for arrays, ranges, expected values
  - `InpRng<U>`: Input range specification (start, stop, type, count)

- **Enumerations**:
  - `TestMode`: UNITTEST, ACCURACY, PERFORMANCE, KNOWNTEST
  - `TestStatus`: PASS/FAIL
  - `RangeType`: Simple, Integer, Fixedval, Random, Linear, Expstep, Bitstep

**Constants**:
```cpp
MAX_IPPTR = 6   // Maximum input parameters
MAX_OPPTR = 2   // Maximum output parameters
MAX_ELEM  = 16  // Maximum SIMD elements
```

---

### 2. **math_api_template.h** - API Prototype System
**Purpose**: Defines API type classification and validation dispatcher

**API Types** (by function signature):
1. `API_PROTOTYPE_01`: 1 input → 1 output (e.g., `sin`, `exp`)
2. `API_PROTOTYPE_02`: 2 inputs → 1 output (e.g., `pow`, `hypot`)
3. `API_PROTOTYPE_03`: 2 inputs (1 scalar) → 1 output (e.g., `powx`)
4. `API_PROTOTYPE_04`: 1 input → 2 outputs (e.g., `sincos`)
5. `API_PROTOTYPE_05`: 6 inputs → 1 output (e.g., `linearfrac`)
6. `API_PROTOTYPE_06`: 2 inputs (1 integer) → 1 output (e.g., `ldexp`)

**Key Functions**:
- `validate_api<T,U>()`: Routes to appropriate prototype handler based on API name mapping
- `process_libm()`: Main entry point for processing YAML test configurations

---

### 3. **generator.h** - Input Range Generators
**Purpose**: Flexible input value generation for range-based tests

**Generator Hierarchy**:
```
IGenerator<S> (abstract base)
├── BitGenerator<S,U>        // Bit-pattern stepping
├── LinearGenerator<S>       // Linear interpolation
├── ExponentialGenerator<S>  // Exponential spacing
├── RandomGenerator<S>       // Random values with RNG
└── MultiStepGenerator<S>    // Composite generator with multiple strategies
```

**Features**:
- Iterator support for range-based loops
- `wrap_next()`: Auto-reset when exhausted
- Type-safe float ↔ uint reinterpretation via `Float2Uint` union
- Alignment support for SIMD requirements

**MultiStepGenerator**:
- Routes to appropriate sub-generator based on `RangeType`
- Handles aligned SIMD input generation
- Supports wrapping/recycling of input sequences

---

### 4. **libm_yaml.h** - YAML I/O Data Structures
**Purpose**: Serialization structures for test configuration and results

**Input Structure** (`YamlInputs`):
```cpp
struct YamlInputs {
    string test_type;          // "unit_test", "range_test"
    string api_name;           // Function name
    string test_id;            // Unique identifier
    string variants;           // "ss;vrs4;vrd2" (semicolon-separated)
    vector<string> input;      // Single input values
    string xv;                 // Expected output
    string xxv;                // Expected exception
    string ulp_threshold;      // Per-variant thresholds
    vector<InputRange> range;  // Range specifications
};
```

**Output Structure** (`YamlOutputs<S>`):
```cpp
template <typename S>
struct YamlOutputs {
    string variant;            // Current variant (ss, sd, vrs4, etc.)
    string api_name;
    string test_id;
    uint64_t n[MAX_IPPTR];     // Element counts
    S *iptr[MAX_IPPTR];        // Input data pointers
    S *optr[MAX_OPPTR];        // Output data pointers
    double *ulp;               // ULP errors
    int *status;               // PASS/FAIL per element
    bool utflag;               // Unit test mode
    int exception_raised;      // FP exception flags
    double ulp_threshold;
    double duration;           // Performance timing
    bool is_vra;               // Vector Real Array mode
    TestMode test_mode;
    string vendor;
    string outfile;
};
```

**Range Specification** (`InputRange`):
- Start/stop values as strings (supports hex, special values)
- Type: linear, expstep, bitstep, random, etc.
- Count: number of values to generate

---

**Key Features**:
- Windows/Clang workaround for `FENV_ACCESS` pragma conflicts
- `safe_accumulate()`: Template wrapper for `std::accumulate`
- Prevents numeric header issues on Windows with strict floating-point control

---

### 6. **packer.h** - SIMD Packing Utilities
**Purpose**: Convert scalar arrays ↔ SIMD types

**Key Components**:
- `align_to(c, e)`: Align count to SIMD boundaries
- `store_val<T,S>()`: Store SIMD type to scalar array
- `FloatPacker<T,S>`: Pack scalar array into SIMD register

**Template Specializations**:
- Scalar: Direct assignment
- SSE: `_mm_loadu_ps/pd`, `_mm_storeu_ps/pd`
- AVX: `_mm256_loadu_ps/pd`, `_mm256_storeu_ps/pd`
- AVX-512: `_mm512_loadu_ps/pd` (conditional on `__AVX512F__`)

**Usage Example**:
```cpp
FloatPacker<AlignedM256> fp;
float arr[8] = {...};
AlignedM256 packed = fp.pack(arr);  // Load 8 floats into __m256
```

---

### 7. **dll_utils.h** - Cross-Platform Abstraction
**Purpose**: Unified dynamic library loading API

**Abstractions**:
- **Library Loading**:
  - Windows: `LoadLibraryA`, `GetProcAddress`, `FreeLibrary`
  - Linux: `dlopen`, `dlsym`, `dlclose`

- **Macros**:
  ```cpp
  DL_HANDLE      // HMODULE or void*
  DL_LOAD(name)
  DL_SYM(lib, name)
  DL_CLOSE(lib)
  DL_ERROR()     // Error string retrieval
  ```

- **Platform-Specific Constants**:
  - `SHARED_LIB_EXT`: `.dll` (Windows) or `.so` (Linux)
  - `PATH_SEPARATOR`: `\\` (Windows) or `/` (Linux)

---

### 8. **test_utils.h** - Test Execution Utilities
**Purpose**: High-level test orchestration and library symbol loading

**Key Functions**:
- `load_function<FuncType>()`: Type-safe symbol loading from shared libraries
- `run_libm_api_with_exceptions()`: Execute function and capture FP exceptions
- `Runner<T,U>` class: Mode-aware execution dispatcher

**Runner Class**:
```cpp
template <typename T, typename U>
class Runner {
    Runner(shim_func, mode, iterations=10);
    double run(InParams<T,U>* ipp);
private:
    double run_perf(InParams<T,U>* ipp);  // Performance mode
    double run_accu(InParams<T,U>* ipp);  // Accuracy mode
};
```

**Features**:
- Performance mode: Multiple iterations, returns minimum time
- Accuracy mode: Single execution, no timing overhead
- Exception tracking via `std::feclearexcept/fetestexcept`

---

### 9. **hrt_utils.h** - Performance Measurement
**Purpose**: High-precision timing for performance benchmarks

**Platform Implementations**:
- **Windows**: `QueryPerformanceCounter` + `QueryPerformanceFrequency`
- **Linux**: `std::chrono::high_resolution_clock`

**API**:
```cpp
timing_wrapper tw;
tw.start();
// ... code to measure ...
double nanoseconds = tw.stop();
```

**Resolution**: Nanosecond precision on both platforms

---

### 10. **ulp.h** - ULP (Unit in Last Place) Calculation
**Purpose**: Floating-point accuracy verification

**Core Structures**:
```cpp
struct ulp_data {
    double max_ulp_err;      // Maximum observed ULP error
    double ulp_threshold;    // Acceptance threshold
};
```

**Key Functions**:
- `update_ulp<S,L>(actual, expected, udata, ulp)`: Compute and check ULP error
- `compute_ulp<S,L>()`: Core ULP calculation
- `ulprep<S>(val)`: Calculate ULP magnitude for a value
- `set_global_ulp_threshold()`: Set default threshold
- `PrintUlpResults*()`: Diagnostic output functions

**ULP Calculation Logic**:
- Handles special cases: NaN, Inf, denormals, overflow
- Uses higher-precision types for reference (float→double, double→long double)
- Returns `TESTCASE_PASS/FAIL` based on threshold comparison

**MPFR Type Mapping**:
```cpp
namespace mpfr {
    template<> struct op_type<float>  { using mopt = double; };
    template<> struct op_type<double> { using mopt = long double; };
}
```

---

## Source Files (`src/`)

### 1. **main.cc** - Test Runner Entry Point
**Purpose**: Command-line interface and orchestration

**Usage**:
```bash
./almbench <shim_library> <yaml_file> [--type|-t <accu|perf>]
```

**Flow**:
1. Parse command-line arguments
2. Load shim library (library under test)
3. Load reference library (MPFR-based high-precision implementations)
4. Read YAML configuration file
5. Call `process_libm()` with test parameters
6. Cleanup and exit

**Features**:
- Auto-loads reference library from `REF_MPARITH` compile-time constant
- Prints loaded library paths (full path on Windows via `GetModuleFileNameA`)
- Supports both accuracy and performance test modes

---

### 2. **almbench_process.cc** - Test Orchestration
**Purpose**: YAML parsing and test dispatch per variant

**Key Functions**:

#### `str2flt<T>(string, T&)` - String to Float Conversion
Handles special values:
- `"max"`, `"min"`: Numeric limits
- `"min_subnormal"`, `"max_subnormal"`: Denormal boundaries
- `"qnan"`, `"snan"`, `"nan"`: NaN variants
- `"inf"`: Infinity
- Hex literals: `"0x3f800000"`
- Decimal/scientific: `"1.5e-10"`

#### `get_exception_flag(string)` - Exception Parsing
Maps strings to FP exception flags:
- `"divbyzero"` → `FE_DIVBYZERO`
- `"invalidoperation"` → `FE_INVALID`
- `"overflow"` → `FE_OVERFLOW`
- `"underflow"` → `FE_UNDERFLOW`
- `"inexact"` → `FE_INEXACT`

Supports semicolon-separated combinations: `"overflow;inexact"`

#### `str_to_enum(string)` - Range Type Mapping
- `"simple"` → `E_Simple`
- `"integer"` → `E_Integer`
- `"linearstep"` → `E_Linear`
- `"expstep"` → `E_Expstep`
- `"bitstep"` → `E_Bitstep`
- etc.

#### `libm_api_variant<T>(...)` - Variant Handler
Template function that:
1. Converts YAML inputs to typed `InParams<T,U>`
2. Sets up `YamlOutputs<U>` structure
3. Populates input ranges or single values
4. Calls `validate_api<T,U>()` dispatcher
5. Cleans up allocated structures

#### `process_libm(...)` - Main Entry Point
For each test in YAML:
1. Parse semicolon-separated variants (e.g., `"ss;vrs4;vrd2"`)
2. For each variant:
   - Route to appropriate type (scalar float, vector float, vector double, etc.)
   - Handle AVX-512 conditionally
   - Skip unsupported variants gracefully

**Variant Mapping**:
- `"ss"` → `libm_api_variant<float>`
- `"sd"` → `libm_api_variant<double>`
- `"vrs4"` → `libm_api_variant<AlignedM128>`
- `"vrs8"` → `libm_api_variant<AlignedM256>`
- `"vrs16"` → `libm_api_variant<AlignedM512>` (if `__AVX512F__`)
- `"vrd2"` → `libm_api_variant<AlignedM128d>`
- `"vrd4"` → `libm_api_variant<AlignedM256d>`
- `"vrd8"` → `libm_api_variant<AlignedM512d>` (if `__AVX512F__`)
- `"vrsa"`, `"vrda"` → VRA (Vector Real Array) modes

---

### 3. **checkapi.cc** - API Routing and Validation
**Purpose**: Map API names to prototype types and dispatch tests

**Key Functions**:

#### `check_outfile_dir<S>(YamlOutputs<S>*)` - Output File Management
- Creates `dumps/<api_name>/` directory structure
- Generates output filename:
  - Unit test: `<vendor>_ut_<api>_<variant>.yaml`
  - Accuracy: `<vendor>_accu_<api>_<variant>.yaml`
  - Performance: `<vendor>_perf_<api>_<variant>.yaml`
- Uses `std::filesystem` for cross-platform path handling

#### `api_prototype_to_string(ApiTypes)` - Enum to String
Converts `API_PROTOTYPE_01` → `"API_PROTOTYPE_01"`, etc.

#### `get_api_name_prototype_map()` - API Name Registry
Static map of function names to prototype types:
```cpp
{
    {"acos", API_PROTOTYPE_01},
    {"sin", API_PROTOTYPE_01},
    {"pow", API_PROTOTYPE_02},
    {"powx", API_PROTOTYPE_03},
    {"sincos", API_PROTOTYPE_04},
    {"linearfrac", API_PROTOTYPE_05},
    {"ldexp", API_PROTOTYPE_06},
    // ... ~50+ functions
}
```

#### `get_api_prototype(string)` - Lookup Function
Returns `ApiTypes` enum for given function name

#### `is_vra_api(string)` - VRA Detection
Checks if variant name ends with `'a'` (e.g., `"vrsa"`, `"vrda"`)

#### `validate_api<T,U>(...)` - Master Dispatcher
**Flow**:
1. Set VRA flag and output file path
2. Look up API prototype type from name
3. Construct library and reference function names with suffixes:
   - Scalar: `_<variant>` (e.g., `sin_ss`, `pow_sd`)
   - Vector: `_<variant>` (e.g., `sin_vrs4`, `pow_vrd2`)
4. Call appropriate `api_prototype_XX<T,U>()`
5. Handle unsupported API errors

**Explicit Instantiations**: All combinations of T × U for:
- `float` × `float`
- `double` × `double`
- `AlignedM128` × `float`
- `AlignedM128d` × `double`
- `AlignedM256` × `float`
- `AlignedM256d` × `double`
- `AlignedM512` × `float` (AVX-512)
- `AlignedM512d` × `double` (AVX-512)

---

### 4. **packer.cc** - SIMD Pack/Unpack Implementation
**Purpose**: Concrete implementations of SIMD data movement

#### `align_to(c, e)` - Alignment Helper
```cpp
uint64_t align_to(uint64_t c, uint64_t e) {
    return (c + (e - 1)) & ~(e - 1);
}
```
Rounds up `c` to next multiple of `e` (e.g., align 13 to 4 → 16)

#### `store_val<T,S>(T val, S arr[])` - SIMD to Array
Template specializations:
- **Scalar**: `arr[0] = val`
- **SSE**: `_mm_storeu_ps(arr, val.data)` (4 floats) or `_mm_storeu_pd` (2 doubles)
- **AVX**: `_mm256_storeu_ps` (8 floats) or `_mm256_storeu_pd` (4 doubles)
- **AVX-512**: `_mm512_storeu_ps` (16 floats) or `_mm512_storeu_pd` (8 doubles)

#### `FloatPacker<T,S>::pack(S arr[])` - Array to SIMD
Inverse of `store_val`:
- **Scalar**: Returns `arr[0]`
- **SSE**: `return {_mm_loadu_ps(&arr[0])}`
- **AVX**: `return {_mm256_loadu_ps(&arr[0])}`
- **AVX-512**: `return {_mm512_loadu_ps(&arr[0])}`

**Explicit Template Instantiations**: All supported SIMD types

---

### 5. **prototype_01.cc** - Single Input, Single Output
**Purpose**: Implements testing for `y = f(x)` functions

**Examples**: `sin`, `cos`, `exp`, `log`, `sqrt`, `asin`, etc.

**Functions**:

#### `unit_test<T,U,UL>(...)`
1. Call `run_libm_api_with_exceptions()` to execute shim function
2. Extract input/output pointers
3. Call reference function: `UL mpfrop = ref_func(ip[0])`
4. Compute ULP error via `update_ulp()`
5. Populate `YamlOutputs` with results
6. Write to YAML

#### `range_test<T,U,UL>(...)` - Non-VRA Range Test
1. Calculate SIMD element count: `elem = sizeof(T) / sizeof(U)`
2. Align input count to SIMD boundary
3. Initialize `Runner` for mode-aware execution
4. Create `MultiStepGenerator` for input range
5. Loop over aligned count:
   - Generate next SIMD-width input batch via `FloatPacker`
   - Execute via `runner.run(ipp)`
   - Compute ULP for each element
   - Write results to YAML

#### `range_test_vra<T,U,UL>(...)` - VRA Range Test
Similar to `range_test`, but:
- Uses pointer-based input (`iptr[0]`) instead of packed SIMD values
- Output to allocated vector (`std::vector<U> op`)
- Limits batch size to 100 elements

**Template Instantiations**: All T × U combinations (6-8 depending on AVX-512)

---

### 6. **prototype_02.cc** - Two Inputs, Single Output
**Purpose**: Implements `z = f(x, y)` functions

**Examples**: `pow`, `hypot`, `atan2`, `fmod`, `remainder`

**Key Differences from Prototype 01**:
- Two `MultiStepGenerator` instances for independent input ranges
- Reference function signature: `UL ref_func(U x, U y)`
- Dual input packing: `ipp->ip[0] = fp.pack(ip1); ipp->ip[1] = fp.pack(ip2)`
- Wrapping behavior: Both generators wrap independently

**VRA Mode**:
- Separate input arrays: `ipp->iptr[0] = ip1; ipp->iptr[1] = ip2`
- Shim function receives pointers to scalar arrays

---

### 7. **prototype_03.cc** - Two Inputs (One Scalar), Single Output
**Purpose**: Implements `z = f(x, s)` where `s` is broadcast scalar

**Examples**: `powx` (where exponent is constant)

**Key Features**:
- First input: SIMD-width generation (`elem` count)
- Second input: Single scalar generation (count = 1)
- Element counts: `yop->n[0] = elem; yop->n[1] = 1`
- Broadcasting: `ipp->ip[1] = T{ip2[0]}`  (scalar to SIMD broadcast)

**Use Case**: Optimized vectorization when one operand is constant across SIMD lanes

---

### 8. **prototype_04.cc** - Single Input, Dual Outputs
**Purpose**: Implements `f(x, &y1, &y2)` functions

**Examples**: `sincos` (computes sine and cosine simultaneously)

**Key Features**:
- Dual output pointers: `ipp->op[0]` and `ipp->op[1]`
- Reference signature: `void ref_func(U x, UL* sin, UL* cos)`
- ULP verification for both outputs: `uflag = update_ulp(ops[j], msin, ...) | update_ulp(opc[j], mcos, ...)`
- Single `ulp` value (maximum of both outputs)

**VRA Mode**:
- Separate output vectors: `std::vector<U> ops(count), opc(count)`
- Pointers passed to shim: `ipp->optr[0] = ops.data(); ipp->optr[1] = opc.data()`

---

### 9. **prototype_05.cc** - Six Inputs, Single Output
**Purpose**: Implements complex multi-parameter functions

**Example**: `linearfrac` - Linear fractional transformation:
```
y[i] = (scalea*a[i] + shifta) / (scaleb*b[i] + shiftb)
```

**Input Breakdown**:
- `ip[0]`: Array `a[]` (SIMD-width or VRA)
- `ip[1]`: Array `b[]` (SIMD-width or VRA)
- `ip[2]`: Scalar `scalea`
- `ip[3]`: Scalar `shifta`
- `ip[4]`: Scalar `scaleb`
- `ip[5]`: Scalar `shiftb`

**Element Counts**:
- `n[0]` = `elem` (SIMD width) or `count` (VRA)
- `n[1]` = `elem` or `count`
- `n[2..5]` = 1 (scalars)

**Range Generation**:
- Six `MultiStepGenerator` instances
- First two: SIMD-aligned or VRA-sized
- Last four: Single values (broadcast)

**Threshold Limitations** (documented in comments):
- Single precision: `EMIN=-126, EMAX=127, p=24`
- Double precision: `EMIN=-1022, EMAX=1023, p=53`
- Range constraints to avoid overflow/underflow

---

### 10. **prototype_06.cc** - Float + Integer Inputs
**Purpose**: Implements `y = f(x, n)` where `n` is integer

**Examples**: `ldexp` (x × 2^n), `scalbn`

**Key Features**:
- Second input stored as float but interpreted as integer
- Type conversion: `int exp = static_cast<int>(std::round(ip2[j]))`
- Reference signature: `UL ref_func(U x, U n_as_float)` (casts internally)

**Use Case**: Functions requiring integer exponents or shift counts

---

### 11. **test_utils.cc** - Utility Function Implementations
**Purpose**: Concrete implementations of test utilities

**Key Implementations**:

#### `load_function<FuncType>(...)` - Type-Safe Symbol Loading
```cpp
template <typename FuncType>
FuncType load_function(DL_HANDLE lib, const string &name) {
    void *symbol = DL_SYM(lib, name.c_str());
    if (!symbol) {
        cerr << "Failed to load: " << name << ": " << DL_ERROR();
        DL_CLOSE(lib);
        exit(EXIT_FAILURE);
    }
    return reinterpret_cast<FuncType>(symbol);
}
```

**Explicit Instantiations**: Function pointer types for:
- Reference functions: `double (*)(float)`, `long double (*)(double)`, etc.
- Shim functions: `void (*)(InParams<T,U>*)`
- Multi-input functions: `double (*)(float, float, float, float, float, float)`

#### `run_libm_api_with_exceptions<T,U>(...)` - Exception Tracking
```cpp
template <typename T, typename U>
int run_libm_api_with_exceptions(shim_func, ipp) {
    std::feclearexcept(FE_ALL_EXCEPT);
    shim_func(ipp);
    int raised = std::fetestexcept(FE_ALL_EXCEPT);
    std::feclearexcept(FE_ALL_EXCEPT);
    return raised;
}
```
Captures: `FE_DIVBYZERO`, `FE_INEXACT`, `FE_INVALID`, `FE_OVERFLOW`, `FE_UNDERFLOW`

#### `Runner<T,U>` Class Implementation
**Constructor**:
```cpp
Runner(void (*shim)(InParams<T,U>*), TestMode mode, uint64_t iterations=10)
    : shim_func(shim), iterations(iterations) {
    run_libm_api = (mode == E_PERFORMANCE) ? &Runner::run_perf
                                            : &Runner::run_accu;
}
```
Uses function pointer to avoid runtime branching in hot loop

**Performance Mode**:
```cpp
double run_perf(InParams<T,U>* ipp) {
    vector<double> durations;
    durations.reserve(iterations);
    for (uint64_t t = 0; t < iterations; ++t) {
        timing_wrapper perf;
        perf.start();
        shim_func(ipp);
        durations.push_back(perf.stop());
    }
    return *min_element(durations.begin(), durations.end());
}
```
Returns minimum time to reduce noise from OS scheduling

**Accuracy Mode**:
```cpp
double run_accu(InParams<T,U>* ipp) {
    shim_func(ipp);
    return 0.0;
}
```
No timing overhead

---

### 12. **hrt_utils.cc** - Platform-Specific Timing
**Purpose**: High-resolution clock implementation

#### Windows Implementation
```cpp
void timing_wrapper::start() {
    QueryPerformanceCounter(&start_time);
}

double timing_wrapper::stop() {
    LARGE_INTEGER end_time, frequency;
    QueryPerformanceCounter(&end_time);
    QueryPerformanceFrequency(&frequency);

    long long elapsed = (end_time.QuadPart - start_time.QuadPart);
    return (double)elapsed * 1e9 / frequency.QuadPart;
}
```

#### Linux Implementation
```cpp
void timing_wrapper::start() {
    start_time = std::chrono::high_resolution_clock::now();
}

double timing_wrapper::stop() {
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> duration = end_time - start_time;
    return duration.count();
}
```

Both return nanoseconds as `double`

---

### 13. **ulp.cc** - ULP Error Calculation
**Purpose**: Floating-point accuracy verification core

#### `FloatTraits<S>` - Type Metadata
```cpp
template <typename S> struct FloatTraits;

template <> struct FloatTraits<float> {
    using uint = uint32_t;
    static constexpr int exponent_bits = 8;
    static constexpr int exponent_bias = 127;
    static constexpr int precision = 24;
};

template <> struct FloatTraits<double> {
    using uint = uint64_t;
    static constexpr int exponent_bits = 11;
    static constexpr int exponent_bias = 1023;
    static constexpr int precision = 53;
};
```

#### `ulprep<S>(val)` - ULP Magnitude Calculation
```cpp
template <typename S>
S ulprep(S val) {
    Float2Uint<S> c = {.f = val};

    int32_t expo = ((c.i >> (digits - 1)) & ((1 << exponent_bits) - 1))
                   - exponent_bias;
    expo = expo - (precision - 1);
    return pow(2, expo);
}
```
Returns the value of 1 ULP at the scale of `val`

#### `compute_ulp<S,L>(actual, expected)` - Core ULP Logic
**Special Case Handling**:
1. **Both NaN**: Return 0.0 (match)
2. **Either NaN**: Return `INFINITY` (mismatch)
3. **Both +Inf** or **Both -Inf**: Return 0.0
4. **Actual finite, Expected ±Inf**: Return distance to `max/lowest` + 1
5. **Actual ±Inf, Expected finite**: Return overflow ULP count

**Normal Case**:
```cpp
return std::abs(actual - expected) / ulprep<S>(expected)
```

#### `update_ulp<S,L>(...)` - Threshold Check
```cpp
template <typename S, typename L>
int update_ulp(S aop, L mpfrop, ulp_data &udata, double &ulp) {
    ulp = compute_ulp<S, L>(aop, mpfrop);
    int res = TESTCASE_PASS;

    if (std::isinf(ulp) || std::isnan(ulp)) {
        res = TESTCASE_FAIL;
    } else {
        udata.max_ulp_err = max(udata.max_ulp_err, ulp);
        if (ulp > udata.ulp_threshold) {
            res = TESTCASE_FAIL;
        }
    }
    return res;
}
```

#### Global Threshold Management
```cpp
namespace {
    static double global_ulp_threshold = 0.5;
}

void set_global_ulp_threshold(double threshold) {
    if (threshold >= 0.0) global_ulp_threshold = threshold;
}

double get_global_ulp_threshold() {
    return global_ulp_threshold;
}
```

**Default Thresholds**:
- Scalar: 0.5 ULP (correctly rounded)
- SIMD: 4.0 ULP (relaxed for vectorization)

---

### 14. **verify.cc** - Diagnostic Output Functions
**Purpose**: Human-readable ULP error reporting

#### `PrintUlpResults01<S,L>(...)` - Single Input
```cpp
template <typename S, typename L>
void PrintUlpResults01(S inp, S ap, L xp, double ulp) {
    cout << "Inp      : " << dec << inp
         << " (0x" << hex << *reinterpret_cast<SUINT*>(&inp) << ")\n";
    cout << "Actual   : " << dec << ap
         << " (0x" << hex << *reinterpret_cast<SUINT*>(&ap) << ")\n";
    cout << "Expected : " << dec << xp
         << " (0x" << hex << *reinterpret_cast<DUINT*>(&xp) << ")\n";
    cout << "ULP      : " << dec << ulp << "\n";
}
```

#### `PrintUlpResults02<S,L>(...)` - Dual Input
Similar, but prints `inp1` and `inp2`

#### `PrintUlpResults<S,L,...>(...)` - Variadic Input
Uses fold expressions (C++17):
```cpp
template <typename S, typename L, typename... Args>
void PrintUlpResults(S ap, L xp, double ulp, Args... inputs) {
    cout << "Inputs: ";
    auto print = [](auto ip) {
        if constexpr (std::is_integral_v<T>) {
            cout << dec << ip << " ";
        } else if constexpr (std::is_floating_point_v<T>) {
            cout << dec << ip << " (0x" << hex << *reinterpret_cast<UINT*>(&ip) << ") ";
        }
    };
    (print(inputs), ...);  // Fold expression
    // ... print actual, expected, ULP
}
```

#### `print_formats<S>(val)` - Multi-Format Display
```cpp
template <typename S>
void print_formats(S val) {
    FloatIntUnion<S,U> u = {.f = val};

    cout << "Value     : " << val << "\n";
    cout << "Binary    : " << bitset<sizeof(S)*8>(u.i) << "\n";
    cout << "Hex       : 0x" << hex << u.i << "\n";
    cout << "Hexfloat  : " << hexfloat << val << "\n";
    cout << "Decimal   : " << defaultfloat << val << "\n";
    cout << "Canonical : " << scientific << val << "\n";
    cout << "Uint      : " << u.i << "\n";
}
```

**Use Cases**: Debugging bit-level representation issues, subnormals, NaN payloads

---

### 15. **yaml_read.cc** - YAML Input Parser
**Purpose**: Parse test configuration from YAML files

#### `read_test(YAML::Node, YamlInputs&)` - Test Case Parser
**Flow**:
1. Extract `id` and optional `description`
2. Parse `input` field:
   - **Sequence (array)**: Treat as range `[start, stop]`
   - **Scalar**: Treat as single unit test value
3. Parse `type`: Range generation strategy (default: `"expstep"`)
4. Parse `steps`: Number of values per range (default: `1000`)
5. Parse `expect`: Expected output value
6. Parse `variants`: Semicolon-join into single string
7. Parse `expect_exception`: Semicolon-join exception names
8. Parse `uth` (ULP threshold): Per-variant thresholds

**ULP Threshold Logic**:
- If `uth` provided: Use specified values
- If missing:
  - Scalar (`ss`, `sd`): Default 0.5
  - Vector/VRA: Default 4.0
- Semicolon-separated for multiple variants

#### `extractApiName(string)` - API Name Extraction
Strips prefixes from test IDs:
- `"accu_sin_test1"` → `"sin"`
- `"perf_pow_range"` → `"pow"`
- `"unittest_exp_nan"` → `"exp"`

Supported prefixes: `"accu_"`, `"perf_"`, `"conf_"`, `"unittest_"`, `"vt_"`

#### `read_yaml_file(filename, vector<YamlInputs>&)` - Main Parser
**YAML Structure**:
```yaml
- test_sequence: "accuracy_tests"
  function: "sin"
  test_sets:
    - id: "range_tests"
      description: "..."
      tests:
        - id: "accu_sin_range1"
          input: [[0.0, 3.14159], [1.0, 2.0]]
          type: ["expstep", "linear"]
          steps: [1000, 500]
          variants: ["ss", "vrs4", "sd"]
          uth: [0.5, 4.0, 0.5]
```

**Parsing**:
1. Iterate `test_sequence` entries
2. For each `test_set`:
   - For each `test`:
     - Call `read_test()`
     - Populate `YamlInputs` struct
     - Append to output vector

**Result**: Vector of `YamlInputs` structs ready for `process_libm()`

---

### 16. **yaml_write.cc** - YAML Output Serialization
**Purpose**: Write test results to YAML format

#### `exception_to_string(int)` - Exception Flag Formatter
```cpp
static string exception_to_string(int raised) {
    string result;
    if (raised & FE_DIVBYZERO) result += "FE_DIVBYZERO ";
    if (raised & FE_INEXACT)   result += "FE_INEXACT ";
    if (raised & FE_INVALID)   result += "FE_INVALID ";
    if (raised & FE_OVERFLOW)  result += "FE_OVERFLOW ";
    if (raised & FE_UNDERFLOW) result += "FE_UNDERFLOW ";
    return result;
}
```

#### `to_hex<S>(value)` - Hex Serialization
```cpp
template <typename S>
static string to_hex(const S &value) {
    stringstream ss;
    ss << "0x" << hex << uppercase << setfill('0');
    if (sizeof(S) == sizeof(uint32_t)) {
        uint32_t bits = *reinterpret_cast<const uint32_t*>(&value);
        ss << setw(8) << bits;
    } else if (sizeof(S) == sizeof(uint64_t)) {
        uint64_t bits = *reinterpret_cast<const uint64_t*>(&value);
        ss << setw(16) << bits;
    }
    return ss.str();
}
```

#### `serialize_yaml_outputs<S>(YamlOutputs<S>*)` - YAML Node Builder
**Output Format**:
```yaml
api: "sin_ss"
n: 4  # SIMD width or VRA count
test_id: "accu_sin_range1"
ip1: [[0x3F800000, 0x40000000, 0x40400000, 0x40800000]]  # Packed as Flow style
op1: [[0x3F576AA4, 0x3F6C835E, 0x3F743682, 0x3F7B14BE]]
ulp: [[0.125, 0.0625, 0.25, 0.1875]]
status: [["PASS", "PASS", "PASS", "PASS"]]
duration: [1234.567]  # Nanoseconds (performance mode only)
exception_raised: ["FE_INEXACT"]  # Unit test mode only
```

**Mode-Specific Behavior**:
- **Unit Test Mode** (`utflag=true`):
  - Single element from each array
  - Include `exception_raised`
  - Print to stdout immediately
- **Batch Mode** (`utflag=false`):
  - All elements serialized
  - Include `duration` (performance mode)
  - No stdout output

**Flow Style**: Uses YAML flow notation (compact arrays) for readability

#### `write_yaml_output<S>(YamlOutputs<S>*)` - File Writer
```cpp
template <typename S>
void write_yaml_output(const YamlOutputs<S> *yop) {
    YAML::Node node = serialize_yaml_outputs<S>(yop);

    ofstream fout(yop->outfile, ios::app);  // Append mode
    fout << node << "\n";
    fout.close();
}
```

**Explicit Instantiations**: `float` and `double`

---

### 17. **generator.cc** - Input Generator Implementations
**Purpose**: Concrete implementations of range generators

#### `BitGenerator<S,U>` - Bit Pattern Stepper
```cpp
template <typename S, typename U>
BitGenerator<S,U>::BitGenerator(S rmin, S rmax, S bitstep)
    : bitstep(bitstep), start{rmin}, stop{rmax}, i(0) {
    bd = abs((int64_t)stop.i - (int64_t)start.i);
    count = bd / bitstep;
    mxiter = count + 1;
}

S* next() {
    value.i = start.i + i * bitstep;
    i++;
    return &value.f;
}
```

**Use Case**: Exhaustive testing of special bit patterns, denormals, NaN payloads

#### `LinearGenerator<S>` - Linear Interpolation
```cpp
template <typename S>
LinearGenerator<S>::LinearGenerator(S rmin, S rmax, uint64_t mxiter)
    : rmin(rmin), rmax(rmax), value(rmin),
      step((rmax - rmin) / mxiter), mxiter(mxiter + 1), i(0) {}

S* next() {
    value = rmin + (i * step);
    i++;
    if (value > rmax) value = rmax;  // Clamp
    return &value;
}
```

**Use Case**: Uniform coverage of a numeric range

#### `ExponentialGenerator<S>` - Exponential Spacing
```cpp
template <typename S>
ExponentialGenerator<S>::ExponentialGenerator(S rmin, S rmax, uint64_t count)
    : rmin(rmin), rmax(rmax), count(count), i(0) {
    if (rmin <= 0 || rmax <= 0) {
        throw invalid_argument("Exponential range must be positive");
    }
    log_start = log(rmin);
    log_end = log(rmax);
    log_step = (log_end - log_start) / count;
}

S* next() {
    value = exp(log_start + i * log_step);
    i++;
    return &value;
}
```

**Use Case**: Better coverage for functions with exponential behavior (e.g., `exp`, `log`)

#### `RandomGenerator<S>` - Uniform Random
```cpp
template <typename S>
RandomGenerator<S>::RandomGenerator(S rmin, S rmax, uint64_t count, uint64_t seed)
    : rmin(rmin), rmax(rmax), count(count), i(0),
      rng(seed), dist(rmin, rmax) {}

S* next() {
    value = dist(rng);
    i++;
    return &value;
}
```

Uses `std::mt19937` (Mersenne Twister) and `std::uniform_real_distribution`

#### `MultiStepGenerator<S>` - Composite Generator
**Constructor Dispatch**:
```cpp
template <typename S>
MultiStepGenerator<S>::MultiStepGenerator(
    S start, S stop, uint64_t count, RangeType type, uint64_t elem) {

    // Align count to element boundary
    uint64_t aligned_count = align_to(count, elem);

    switch (type) {
        case E_Simple:
        case E_Linear:
            generator = make_unique<LinearGenerator<S>>(start, stop, aligned_count);
            break;
        case E_Expstep:
            generator = make_unique<ExponentialGenerator<S>>(start, stop, aligned_count);
            break;
        case E_Bitstep:
            // Default bitstep = 1
            generator = make_unique<BitGenerator<S>>(start, stop, 1);
            break;
        case E_Random:
            // Default seed = 42
            generator = make_unique<RandomGenerator<S>>(start, stop, aligned_count, 42);
            break;
        default:
            throw invalid_argument("Unsupported RangeType");
    }
}
```

**Delegation**: All `IGenerator` interface calls forwarded to owned sub-generator

**Explicit Template Instantiations**: `float` and `double`

---

## Testing Workflow

### 1. YAML Input Creation
```yaml
- test_sequence: "accuracy_tests"
  function: "sin"
  test_sets:
    - id: "range_tests"
      tests:
        - id: "accu_sin_full_range"
          input: [[-3.14159, 3.14159]]
          type: ["expstep"]
          steps: [10000]
          variants: ["ss", "vrs4", "sd", "vrd2"]
          uth: [0.5, 4.0, 0.5, 4.0]
```

### 2. Execution
```bash
./almbench libaocl_shim.so sin_tests.yaml --type accu
```

### 3. Processing Flow
```
main.cc
  ├─> Load libraries (shim + reference)
  ├─> read_yaml_file() → vector<YamlInputs>
  └─> process_libm()
       ├─> For each variant (ss, vrs4, sd, vrd2):
       │    ├─> libm_api_variant<T>()
       │    │    ├─> Convert YAML → InParams<T,U>
       │    │    ├─> Create YamlOutputs<U>
       │    │    └─> validate_api<T,U>()
       │    │         ├─> Look up API type (PROTOTYPE_01)
       │    │         ├─> Build symbol names (sin_ss, sin_vrs4, etc.)
       │    │         └─> api_prototype_01<T,U>()
       │    │              ├─> Load function symbols
       │    │              ├─> range_test() or unit_test()
       │    │              │    ├─> MultiStepGenerator → inputs
       │    │              │    ├─> Runner::run() → execute
       │    │              │    ├─> update_ulp() → verify
       │    │              │    └─> write_yaml_output()
       │    │              └─> Return status
       │    └─> Cleanup
       └─> Next variant
```

### 4. Output Files
```
dumps/sin/
  ├─ amd_accu_sin_ss.yaml
  ├─ amd_accu_sin_vrs4.yaml
  ├─ amd_accu_sin_sd.yaml
  └─ amd_accu_sin_vrd2.yaml
```

### 5. Output Content Example
```yaml
api: "sin_vrs4"
n: 4
test_id: "accu_sin_full_range"
ip1: [[0xC0490FDB, 0xBF490FDB, 0x3F490FDB, 0x40490FDB]]
op1: [[0xBF800000, 0xBF000000, 0x3F000000, 0x3F800000]]
ulp: [[0.125, 0.0625, 0.0625, 0.125]]
status: [["PASS", "PASS", "PASS", "PASS"]]
---
api: "sin_vrs4"
n: 4
...
```

---

## Key Design Decisions

### 1. Template-Heavy Architecture
**Rationale**:
- Compile-time polymorphism avoids virtual function overhead
- Explicit instantiations ensure all code is generated at build time
- Type safety: Compiler catches SIMD/scalar mismatches

**Trade-off**: Longer compile times, larger binary size

### 2. Separate Accuracy and Performance Modes
**Rationale**:
- Accuracy tests need ULP verification (slow reference functions)
- Performance tests need minimal overhead (skip ULP calculation)

**Implementation**: `Runner` class uses function pointer selected at construction

### 3. YAML for Configuration and Results
**Rationale**:
- Human-readable test definitions
- Easy integration with CI/CD pipelines
- Structured data for post-processing (Python scripts)

**Alternative Considered**: JSON (rejected due to verbosity)

### 4. Multiple Prototype Handlers
**Rationale**:
- Avoid complex generic code for all signatures
- Each prototype optimized for its use case
- Clear mapping from function signature to implementation

**Alternative Considered**: Single generic handler with type erasure (rejected: performance concerns)

### 5. VRA (Vector Real Array) Mode
**Rationale**:
- Some APIs work on arbitrary-length arrays, not SIMD registers
- Matches vendor-specific extensions (e.g., Intel's `vmlXXXa` functions)

**Implementation**: Separate code paths in each prototype

### 6. Hexadecimal Output
**Rationale**:
- Bit-exact comparison across platforms
- Avoids decimal rounding issues in YAML parsing
- Essential for denormal/NaN debugging

**Format**: IEEE 754 bit representation (e.g., `0x3F800000` = 1.0f)

### 7. Global ULP Threshold
**Rationale**:
- Allows runtime adjustment without recompilation
- Useful for investigating borderline failures

**Implementation**: Static variable with getter/setter

### 8. Exception Tracking
**Rationale**:
- Floating-point exceptions are part of IEEE 754 standard
- Some functions must raise specific exceptions (e.g., `log(-1)` → `FE_INVALID`)

**Implementation**: `std::feclearexcept/fetestexcept` around function calls

---

## Platform-Specific Considerations

### Windows
- **Dynamic Loading**: `LoadLibraryA` requires explicit `.dll` extension
- **Timing**: `QueryPerformanceCounter` for high resolution
- **Paths**: Backslash separators (`\\`)

### Linux
- **Dynamic Loading**: `dlopen` with `RTLD_LAZY` flag
- **Timing**: `std::chrono::high_resolution_clock`
- **Paths**: Forward slash separators (`/`)
- **FENV_ACCESS**: Native support, no issues

### AVX-512
- Conditional compilation: `#ifdef __AVX512F__`
- Template instantiations guarded
- Graceful fallback for unsupported platforms

---

## Testing Coverage

### Supported API Types
1. **Unary**: sin, cos, exp, log, sqrt, cbrt, erf, etc.
2. **Binary**: pow, hypot, atan2, fmod, remainder, etc.
3. **Special**: sincos, linearfrac, ldexp, modf
4. **Variants**:
   - Scalar single: `_ss`
   - Scalar double: `_sd`
   - Vector float: `_vrs4` (SSE), `_vrs8` (AVX), `_vrs16` (AVX-512)
   - Vector double: `_vrd2` (SSE), `_vrd4` (AVX), `_vrd8` (AVX-512)
   - VRA float: `_vrsa`
   - VRA double: `_vrda`

### Test Types
1. **Unit Tests**: Single input/output pairs with expected exceptions
2. **Range Tests**: Exhaustive coverage over numeric ranges
3. **Performance Tests**: Throughput and latency measurements
4. **Conformance Tests**: Standards compliance (IEEE 754, C99)

### Range Generation Strategies
1. **Linear**: Uniform spacing
2. **Exponential**: Log-uniform spacing
3. **Bitstep**: Exhaustive bit-pattern coverage
4. **Random**: Monte Carlo testing
5. **Custom**: User-defined sequences

---

## Integration Points

### External Dependencies
- **yaml-cpp**: YAML parsing and emitting
- **MPFR Library**: High-precision reference implementations (via `mparith32/64`)
- **Intrinsics**: SSE/AVX/AVX-512 headers (`<immintrin.h>`)

### Build System Integration
- CMake support expected (references to `REF_MPARITH` macro)
- Shared library loading (`.so` or `.dll`)
- Platform detection (`_WIN32`, `__AVX512F__`)

### CI/CD Integration
- YAML output suitable for parsing by test frameworks
- Exit codes: 0 (success), non-zero (failure)
- Structured logging for automated analysis

---

## Extensibility

### Adding New API Types
1. Create `prototype_XX.cc` with signature-specific logic
2. Add enum value to `ApiTypes`
3. Register in `get_api_name_prototype_map()`
4. Add case in `validate_api<T,U>()`

### Adding New SIMD ISAs
1. Define new aligned types (e.g., `AlignedNEON`)
2. Add `type_info` specialization
3. Implement packer specializations
4. Add explicit template instantiations

### Adding New Generators
1. Inherit from `IGenerator<S>`
2. Implement `next()`, `has_next()`, `reset()`, `get_index()`
3. Add case to `MultiStepGenerator` constructor
4. Add enum value to `RangeType`

---

## Performance Characteristics

### Memory Allocation
- **Unit Tests**: Stack-allocated `InParams`, heap `YamlOutputs`
- **Range Tests**: Heap-allocated input/output buffers (aligned for SIMD)
- **VRA Tests**: `std::vector` for dynamic sizing

### Computational Complexity
- **ULP Calculation**: O(1) per element
- **Range Test**: O(n) where n = input count
- **Performance Mode**: O(n × iterations)

### Optimizations
- **Alignment**: Input arrays aligned to SIMD boundaries
- **Prefetching**: Implicit via sequential access patterns
- **Branch Prediction**: Mode selection via function pointers (not runtime `if`)
- **Inlining**: Template instantiations enable compiler optimizations

---

## Error Handling

### Failure Modes
1. **Symbol Not Found**: Library doesn't export expected function → exit with error
2. **Invalid Range**: Exponential generator with negative values → exception
3. **File I/O Error**: Cannot create output directory → printed warning, continues
4. **YAML Parse Error**: Malformed input file → exception from yaml-cpp

### Diagnostics
- `PrintUlpResults*()`: Detailed mismatch reporting
- `print_formats()`: Multi-representation value display
- Exception strings: Human-readable flag names

### Logging
- Verbose mode flag (extern bool `verbose`)
- stdout: Test progress and summary
- stderr: Errors and warnings

---

## Maintenance Considerations

### Code Duplication
- **Prototype files**: Similar structure across 01-06 (intentional for clarity)
- **Template instantiations**: Verbose but necessary for link-time resolution

### Testing the Test Suite
- Unit tests for generators (`BitGenerator`, `LinearGenerator`)
- ULP calculation verification against known cases
- Cross-platform validation (Windows vs. Linux)

### Documentation Debt
- Some magic numbers in `prototype_05.cc` (linearfrac thresholds)
- Platform-specific quirks not fully documented
- YAML schema not formalized

---

## Summary Statistics

### Code Metrics
- **Header Files**: 10
- **Source Files**: 17
- **Total SLOC**: ~8,000 (estimated, excluding comments)
- **Template Instantiations**: ~150 explicit
- **Supported Types**: 8 (scalar + SIMD combinations)
- **API Prototypes**: 6 distinct signatures

### Test Coverage
- **Functions Tested**: 50+ (from API map)
- **Variants per Function**: 6-8 (depending on AVX-512)
- **Total Test Combinations**: 300+ (functions × variants)

### Platform Support
- **Windows**: Full support (MSVC, Clang)
- **Linux**: Full support (GCC, Clang)
- **macOS**: Likely works (untested, uses POSIX APIs)

---

## Recommendations for Future Work

### Immediate
1. **Formalize YAML Schema**: XSD or JSON Schema for validation
2. **Add Generator Unit Tests**: Verify correct sequence generation
3. **Performance Baseline Database**: Track regressions over time

### Medium-Term
1. **ARM NEON Support**: Extend to ARM architectures
2. **Multi-Threading**: Parallel test execution for large ranges
3. **Result Aggregation**: Consolidate per-variant YAMLs into summary

### Long-Term
1. **Automatic API Discovery**: Parse headers instead of hardcoded maps
2. **Fuzz Testing Integration**: Use generators for continuous fuzzing
3. **Visual Dashboard**: Web UI for result visualization

---

## Conclusion

The AMD LibM test suite is a robust, well-architected framework for comprehensive math library validation. Its template-based design provides type safety and performance, while YAML-driven configuration ensures flexibility. The separation of accuracy and performance concerns, combined with detailed ULP tracking, makes it suitable for both development and release qualification.

Key strengths:
- **Comprehensive Coverage**: Scalar, SIMD, and VRA variants
- **Precision Verification**: Bit-exact ULP calculations
- **Platform Portability**: Windows and Linux support
- **Extensibility**: Clean abstraction for new APIs and ISAs

Areas for improvement:
- **Documentation**: More inline comments, formal schemas
- **Testing**: Self-tests for generator correctness
- **Tooling**: Automated result analysis scripts

Overall, this is production-grade testing infrastructure that demonstrates careful consideration of numerical accuracy, performance measurement, and cross-platform compatibility.

---

**End of Summary**
