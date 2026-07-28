#!/bin/bash
#
# compare_utils.sh - Compare CPUID results between internal and external utils
#
# This script builds and runs the CPUID comparison test against both:
#   1. Internal alm_utils (from this repository)
#   2. External aocl-utils (if installed on the system)
#
# Usage:
#   ./compare_utils.sh [build_dir]
#
# Arguments:
#   build_dir   Path to CMake build directory (default: ../../build)
#
# Requirements:
#   - CMake build must have been configured with -DALM_TEST_AGAINST_AOCLUTILS=ON
#   - Or: manually compiled test executables in the build directory
#
# Example:
#   mkdir -p build && cd build
#   cmake .. -DALM_TEST_AGAINST_AOCLUTILS=ON
#   make test_cpuid_compare_internal test_cpuid_compare_external
#   cd ../cpuid_tests
#   ./compare_utils.sh ../build
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${1:-${SCRIPT_DIR}/../build}"

# Output files
INTERNAL_OUTPUT="${SCRIPT_DIR}/cpuid_internal.txt"
EXTERNAL_OUTPUT="${SCRIPT_DIR}/cpuid_external.txt"

echo "=== CPUID Utils Comparison Test ==="
echo ""
echo "Script directory: ${SCRIPT_DIR}"
echo "Build directory:  ${BUILD_DIR}"
echo ""

# Find internal utils test executable
INTERNAL_EXE=""
for path in \
    "${BUILD_DIR}/cpuid_tests/test_cpuid_compare_internal" \
    "${BUILD_DIR}/tests/test_cpuid_compare_internal" \
    "${BUILD_DIR}/test_cpuid_compare_internal"; do
    if [ -x "$path" ]; then
        INTERNAL_EXE="$path"
        break
    fi
done

if [ -z "$INTERNAL_EXE" ]; then
    echo "ERROR: Internal utils test executable not found"
    echo "Please build with: cmake -DALM_TEST_AGAINST_AOCLUTILS=ON .. && make test_cpuid_compare_internal"
    exit 1
fi

echo "Internal utils executable: ${INTERNAL_EXE}"

# Run internal utils test
echo "Running internal utils test..."
if ! "${INTERNAL_EXE}" > "${INTERNAL_OUTPUT}" 2>&1; then
    echo "ERROR: Internal utils test failed"
    cat "${INTERNAL_OUTPUT}"
    exit 1
fi
echo "  Output saved to: ${INTERNAL_OUTPUT}"

# Find external aocl-utils test executable
EXTERNAL_EXE=""
for path in \
    "${BUILD_DIR}/cpuid_tests/test_cpuid_compare_external" \
    "${BUILD_DIR}/tests/test_cpuid_compare_external" \
    "${BUILD_DIR}/test_cpuid_compare_external"; do
    if [ -x "$path" ]; then
        EXTERNAL_EXE="$path"
        break
    fi
done

if [ -z "$EXTERNAL_EXE" ]; then
    echo ""
    echo "WARNING: External aocl-utils test executable not found"
    echo "Skipping comparison (external aocl-utils may not be installed)"
    echo ""
    echo "To enable comparison:"
    echo "  1. Install aocl-utils on the system"
    echo "  2. Build with: cmake -DALM_TEST_AGAINST_AOCLUTILS=ON .."
    echo ""
    echo "=== Internal utils output ==="
    cat "${INTERNAL_OUTPUT}"
    exit 0
fi

echo "External aocl-utils executable: ${EXTERNAL_EXE}"

# Run external aocl-utils test
echo "Running external aocl-utils test..."
if ! "${EXTERNAL_EXE}" > "${EXTERNAL_OUTPUT}" 2>&1; then
    echo "ERROR: External aocl-utils test failed"
    cat "${EXTERNAL_OUTPUT}"
    exit 1
fi
echo "  Output saved to: ${EXTERNAL_OUTPUT}"

# Compare outputs (excluding comment lines)
echo ""
echo "Comparing outputs..."
echo ""

# Filter out comment lines for comparison
grep -v "^#" "${INTERNAL_OUTPUT}" > "${INTERNAL_OUTPUT}.filtered" || true
grep -v "^#" "${EXTERNAL_OUTPUT}" > "${EXTERNAL_OUTPUT}.filtered" || true

if diff -q "${INTERNAL_OUTPUT}.filtered" "${EXTERNAL_OUTPUT}.filtered" > /dev/null 2>&1; then
    echo "=== COMPARISON PASSED ==="
    echo ""
    echo "Internal utils and external aocl-utils produce identical results!"
    echo ""
    echo "Output (internal utils):"
    cat "${INTERNAL_OUTPUT}"
    rm -f "${INTERNAL_OUTPUT}.filtered" "${EXTERNAL_OUTPUT}.filtered"
    exit 0
else
    echo "=== COMPARISON FAILED ==="
    echo ""
    echo "Differences found between internal and external utils:"
    echo ""
    diff -u "${INTERNAL_OUTPUT}" "${EXTERNAL_OUTPUT}" || true
    echo ""
    rm -f "${INTERNAL_OUTPUT}.filtered" "${EXTERNAL_OUTPUT}.filtered"
    exit 1
fi
