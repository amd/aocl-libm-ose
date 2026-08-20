#
# Utils module tests
#
# The internal CPU-detection sources are embedded into libalm, so the tests that
# exercise the public CPU-detection API link the self-contained libalm instead
# of recompiling alm_cpuid.c. Each such test is built in two variants: one
# linking the shared libalm (libm_shared -> libalm.so) and one linking the
# static libalm (libm_static -> libalm.a), so both delivery forms are covered.
#
# Include dirs are scoped per-target (target_include_directories) rather than a
# directory-wide include_directories(), so the optional external comparison test
# (test_cpuid_compare_external) resolves <alci/arch.h> from the external
# aocl-utils package instead of the internal header under src/utils.

# Internal utils public header (alm_arch.h) lives with the object sources.
# Use libm's own source root (ALM_SOURCE_DIR, i.e. ${PROJECT_PREFIX}_SOURCE_DIR)
# rather than the top-level CMake source dir, so the path stays correct when
# libm is built as a subproject (build-in-yours).
set(UTILS_TEST_INCLUDE ${${PROJECT_PREFIX}_SOURCE_DIR}/src/utils)

# Build shared- and static-linked variants of a libalm-linked cpuid test.
# The shared variant sets BUILD_RPATH to the libalm.so output dir so ctest can
# resolve the shared library at run time without LD_LIBRARY_PATH.
function(alm_add_cpuid_test base src)
    # libalm pulls in libm math symbols (e.g. cabs); link the system math
    # library so the shared object's references resolve at link time. The Unix
    # math library "m" does not exist on Windows/MSVC (math lives in the CRT),
    # so guard it the same way the rest of the tree links "m".
    set(ALM_CPUID_MATH_LIB "")
    if(NOT WIN32)
        set(ALM_CPUID_MATH_LIB m)
    endif()
    if(TARGET libm_shared)
        add_executable(${base}_shared ${src})
        target_include_directories(${base}_shared PRIVATE ${UTILS_TEST_INCLUDE})
        target_link_libraries(${base}_shared PRIVATE libm_shared ${ALM_CPUID_MATH_LIB})
        set_target_properties(${base}_shared PROPERTIES
            BUILD_RPATH "$<TARGET_FILE_DIR:libm_shared>")
        add_test(NAME ${base}_shared COMMAND ${base}_shared)
    endif()
    if(TARGET libm_static)
        add_executable(${base}_static ${src})
        target_include_directories(${base}_static PRIVATE ${UTILS_TEST_INCLUDE})
        target_link_libraries(${base}_static PRIVATE libm_static ${ALM_CPUID_MATH_LIB})
        add_test(NAME ${base}_static COMMAND ${base}_static)
    endif()
endfunction()

# Tests that exercise the public CPU-detection API link the self-contained libalm.
alm_add_cpuid_test(test_cpuid test_cpuid.c)
alm_add_cpuid_test(test_mock_cpuid test_mock_cpuid.c)
alm_add_cpuid_test(test_dispatch_verify test_dispatch_verify.c)

# Branch-coverage test is fully self-contained: it re-implements the detection
# logic to exercise every branch, so it needs neither libalm nor alm_cpuid.c.
add_executable(test_cpuid_branches test_cpuid_branches.c)
add_test(NAME test_cpuid_branches COMMAND test_cpuid_branches)

#
# CPUID comparison test - compares internal utils against external aocl-utils
#
# This test verifies that our internal CPUID implementation produces the same
# results as the external aocl-utils library.
#
option(ALM_TEST_AGAINST_AOCLUTILS "Build comparison test against external aocl-utils" OFF)

# Internal-utils version of the comparison test links the self-contained libalm
# (shared + static variants, like the other API tests above).
alm_add_cpuid_test(test_cpuid_compare_internal test_cpuid_compare.c)

# Pick an existing internal comparison variant (shared preferred) for the
# optional external diff test below.
if(TARGET test_cpuid_compare_internal_shared)
    set(cpuid_compare_internal_tgt test_cpuid_compare_internal_shared)
elseif(TARGET test_cpuid_compare_internal_static)
    set(cpuid_compare_internal_tgt test_cpuid_compare_internal_static)
endif()

if(ALM_TEST_AGAINST_AOCLUTILS)
    # Try to find external aocl-utils via pkg-config
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(AOCLUTILS QUIET aocl-utils)
    endif()

    # Fallback: check common install locations
    if(NOT AOCLUTILS_FOUND)
        find_path(AOCLUTILS_INCLUDE_DIR alci/arch.h
            PATHS /usr/local/include /usr/include
            NO_DEFAULT_PATH)
        find_library(AOCLUTILS_LIBRARY aoclutils
            PATHS /usr/local/lib /usr/lib
            NO_DEFAULT_PATH)
        if(AOCLUTILS_INCLUDE_DIR AND AOCLUTILS_LIBRARY)
            set(AOCLUTILS_FOUND TRUE)
            set(AOCLUTILS_INCLUDE_DIRS ${AOCLUTILS_INCLUDE_DIR})
            set(AOCLUTILS_LIBRARIES ${AOCLUTILS_LIBRARY})
        endif()
    endif()

    if(AOCLUTILS_FOUND AND cpuid_compare_internal_tgt)
        message(STATUS "External aocl-utils found, building comparison test")
        message(STATUS "  Include: ${AOCLUTILS_INCLUDE_DIRS}")
        message(STATUS "  Library: ${AOCLUTILS_LIBRARIES}")

        # Build external aocl-utils version of the comparison test. Include dirs
        # are scoped to this target only, so it resolves the EXTERNAL header.
        add_executable(test_cpuid_compare_external test_cpuid_compare.c)
        target_compile_definitions(test_cpuid_compare_external PRIVATE USE_EXTERNAL_AOCLUTILS)
        target_include_directories(test_cpuid_compare_external PRIVATE ${AOCLUTILS_INCLUDE_DIRS})
        target_link_libraries(test_cpuid_compare_external PRIVATE ${AOCLUTILS_LIBRARIES})
        add_test(NAME test_cpuid_compare_external COMMAND test_cpuid_compare_external)

        # Add a comparison test that runs both and diffs the output
        add_test(NAME test_cpuid_compare_diff
            COMMAND ${CMAKE_COMMAND}
                -DINTERNAL_EXE=$<TARGET_FILE:${cpuid_compare_internal_tgt}>
                -DEXTERNAL_EXE=$<TARGET_FILE:test_cpuid_compare_external>
                -P ${CMAKE_CURRENT_SOURCE_DIR}/compare_cpuid_outputs.cmake)
        set_tests_properties(test_cpuid_compare_diff PROPERTIES
            DEPENDS "${cpuid_compare_internal_tgt};test_cpuid_compare_external")
    else()
        message(WARNING "External aocl-utils not found, skipping comparison test")
        message(WARNING "Install aocl-utils or set PKG_CONFIG_PATH to enable comparison")
    endif()
endif()

# Unique convenience target to build all internal utils tests at once.
set(cpuid_test_targets test_cpuid_branches)
foreach(base test_cpuid test_mock_cpuid test_dispatch_verify test_cpuid_compare_internal)
    foreach(variant shared static)
        if(TARGET ${base}_${variant})
            list(APPEND cpuid_test_targets ${base}_${variant})
        endif()
    endforeach()
endforeach()
add_custom_target(cpuid_tests_all DEPENDS ${cpuid_test_targets})
