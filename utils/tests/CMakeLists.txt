#
# Utils module tests
#

# Static library tests
add_executable(test_cpuid test_cpuid.c)
target_link_libraries(test_cpuid PRIVATE alm_utils_static)
add_test(NAME test_cpuid COMMAND test_cpuid)

add_executable(test_mock_cpuid test_mock_cpuid.c)
target_link_libraries(test_mock_cpuid PRIVATE alm_utils_static)
add_test(NAME test_mock_cpuid COMMAND test_mock_cpuid)

add_executable(test_dispatch_verify test_dispatch_verify.c)
target_link_libraries(test_dispatch_verify PRIVATE alm_utils_static)
add_test(NAME test_dispatch_verify COMMAND test_dispatch_verify)

# Branch coverage test (standalone, no library link needed)
add_executable(test_cpuid_branches test_cpuid_branches.c)
add_test(NAME test_cpuid_branches COMMAND test_cpuid_branches)

#
# CPUID comparison test - compares internal utils against external aocl-utils
#
# This test verifies that our internal CPUID implementation produces the same
# results as the external aocl-utils library.
#
option(ALM_TEST_AGAINST_AOCLUTILS "Build comparison test against external aocl-utils" OFF)

# Always build the internal utils version of the comparison test
add_executable(test_cpuid_compare_internal test_cpuid_compare.c)
target_link_libraries(test_cpuid_compare_internal PRIVATE alm_utils_static)
add_test(NAME test_cpuid_compare_internal COMMAND test_cpuid_compare_internal)

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

    if(AOCLUTILS_FOUND)
        message(STATUS "External aocl-utils found, building comparison test")
        message(STATUS "  Include: ${AOCLUTILS_INCLUDE_DIRS}")
        message(STATUS "  Library: ${AOCLUTILS_LIBRARIES}")

        # Build external aocl-utils version of the comparison test
        add_executable(test_cpuid_compare_external test_cpuid_compare.c)
        target_compile_definitions(test_cpuid_compare_external PRIVATE USE_EXTERNAL_AOCLUTILS)
        target_include_directories(test_cpuid_compare_external PRIVATE ${AOCLUTILS_INCLUDE_DIRS})
        target_link_libraries(test_cpuid_compare_external PRIVATE ${AOCLUTILS_LIBRARIES})
        add_test(NAME test_cpuid_compare_external COMMAND test_cpuid_compare_external)

        # Add a comparison test that runs both and diffs the output
        add_test(NAME test_cpuid_compare_diff
            COMMAND ${CMAKE_COMMAND}
                -DINTERNAL_EXE=$<TARGET_FILE:test_cpuid_compare_internal>
                -DEXTERNAL_EXE=$<TARGET_FILE:test_cpuid_compare_external>
                -P ${CMAKE_CURRENT_SOURCE_DIR}/compare_cpuid_outputs.cmake)
        set_tests_properties(test_cpuid_compare_diff PROPERTIES
            DEPENDS "test_cpuid_compare_internal;test_cpuid_compare_external")
    else()
        message(WARNING "External aocl-utils not found, skipping comparison test")
        message(WARNING "Install aocl-utils or set PKG_CONFIG_PATH to enable comparison")
    endif()
endif()
