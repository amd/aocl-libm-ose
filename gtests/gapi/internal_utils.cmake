# Defines functions and macros useful for building Google Test and
# Google Mock.
#
# Note:
#
# - This file will be run twice when building Google Mock (once via
#   Google Test's CMakeLists.txt, and once via Google Mock's).
#   Therefore it shouldn't have any side effects other than defining
#   the functions and macros.
#
# - The functions/macros defined in this file may depend on Google
#   Test and Google Mock's option() definitions, and thus must be
#   called *after* the options have been defined.

if (POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif (POLICY CMP0054)

# Defines the compiler/linker flags used to build Google Test and
# Google Mock.  You can tweak these definitions to suit your need.  A
# variable's value is empty before it's explicitly assigned to.
macro(gtest_config_compiler_and_linker)
  # Note: pthreads on MinGW is not supported, even if available
  # instead, we use windows threading primitives
  unset(GTEST_HAS_PTHREAD)
  if (NOT gtest_disable_pthreads AND NOT MINGW)
    # Defines CMAKE_USE_PTHREADS_INIT and CMAKE_THREAD_LIBS_INIT.
    find_package(Threads)
    if (CMAKE_USE_PTHREADS_INIT)
      set(GTEST_HAS_PTHREAD ON)
    endif()
  endif()

  set(cxx_base_flags "-O2")  
  set(cxx_base_flags "${cxx_base_flags} -Wall -Wshadow -Werror")
  if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0.0)
    set(cxx_base_flags "${cxx_base_flags} -Wno-error=dangling-else")
  endif()
  set(cxx_exception_flags "-fexceptions")
  set(cxx_no_exception_flags "-fno-exceptions")
  # Until version 4.3.2, GCC doesn't define a macro to indicate
  # whether RTTI is enabled.  Therefore we define GTEST_HAS_RTTI
  # explicitly.
  set(cxx_no_rtti_flags "-fno-rtti -DGTEST_HAS_RTTI=0")
  set(cxx_strict_flags
    "-Wextra -Wno-unused-parameter -Wno-missing-field-initializers")

  # The pthreads library is available and allowed?
  if (DEFINED GTEST_HAS_PTHREAD)
    set(GTEST_HAS_PTHREAD_MACRO "-DGTEST_HAS_PTHREAD=1")
  else()
    set(GTEST_HAS_PTHREAD_MACRO "-DGTEST_HAS_PTHREAD=0")
  endif()
  set(cxx_base_flags "${cxx_base_flags} ${GTEST_HAS_PTHREAD_MACRO}")

  # For building gtest's own tests and samples.
  set(cxx_exception "${cxx_base_flags} ${cxx_exception_flags}")
  set(cxx_no_exception
    "${CMAKE_CXX_FLAGS} ${cxx_base_flags} ${cxx_no_exception_flags}")
  set(cxx_default "${cxx_exception}")

  # For building the gtest libraries.
  set(cxx_gtest "${cxx_default} ${cxx_strict_flags}")
endmacro()

macro(gbench_config_compiler_and_linker)
  set(cxx_base_flags "-O3")  
  set(cxx_base_flags "${cxx_base_flags} -Wall -Wshadow -Werror")
  set(cxx_base_flags "${cxx_base_flags} -std=c++17 -DNDEBUG")  
  set(cxx_base_flags "${cxx_base_flags} -fstrict-aliasing -Wno-deprecated-declarations ")    
  set(cxx_strict_flags "-Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
  set(cxx_macros "-DHAVE_POSIX_REGEX -DHAVE_STD_REGEX -DHAVE_STEADY_CLOCK")
  
  # For building the gbench libraries.
  set(cxx_bench "${cxx_base_flags} ${cxx_strict_flags} ${cxx_macros}")
endmacro()


# Defines the gtest & gtest_main libraries.  User tests should link
# with one of them.
function(cxx_library_with_type name type cxx_flags)
  # type can be either STATIC or SHARED to denote a static or shared library.
  # ARGN refers to additional arguments after 'cxx_flags'.
  add_library(${name} ${type} ${ARGN})
  set_target_properties(${name}
    PROPERTIES
    COMPILE_FLAGS "${cxx_flags}")
  # Generate debug library name with a postfix.
  set_target_properties(${name}
    PROPERTIES
    DEBUG_POSTFIX "d")
  # Set the output directory for build artifacts
  set_target_properties(${name}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
  # make PDBs match library name
  get_target_property(pdb_debug_postfix ${name} DEBUG_POSTFIX)
  set_target_properties(${name}
    PROPERTIES
    PDB_NAME "${name}"
    PDB_NAME_DEBUG "${name}${pdb_debug_postfix}"
    COMPILE_PDB_NAME "${name}"
    COMPILE_PDB_NAME_DEBUG "${name}${pdb_debug_postfix}")

  if (DEFINED GTEST_HAS_PTHREAD)
    if ("${CMAKE_VERSION}" VERSION_LESS "3.1.0")
      set(threads_spec ${CMAKE_THREAD_LIBS_INIT})
    else()
      set(threads_spec Threads::Threads)
    endif()
    target_link_libraries(${name} PUBLIC ${threads_spec})
  endif()
endfunction()


macro(config_compiler_and_linker)
message("C++ compiler flags: ${CMAKE_CXX_FLAGS}")
  set(cxx_base_flags "-O3 -std=c++17")  
  set(cxx_base_flags "${cxx_base_flags} -march=znver1")
  set(cxx_base_flags "${cxx_base_flags} -fno-rtti -fno-omit-frame-pointer")
  set(cxx_base_flags "${cxx_base_flags} -falign-functions=32 -funsigned-char")
  set(cxx_base_flags "${cxx_base_flags} -fno-strict-aliasing -fstack-protector-all")
  set(cxx_base_flags "${cxx_base_flags} -Wno-multichar -Wno-unused-parameter")
  set(cxx_base_flags "${cxx_base_flags} -Wno-missing-field-initializers -fpic")
  set(cxx_macros "-DLIBM_PROTOTYPE=PROTOTYPE_AOCL")

  set(CMAKE_CXX_FLAGS "${cxx_base_flags} ${cxx_macros}")
endmacro()

########################################################################
#
# Helper functions for creating build targets.

function(gtest_shared_library name cxx_flags)
  cxx_library_with_type(${name} SHARED "${cxx_flags}" ${ARGN})
endfunction()

function(gtest_library name cxx_flags)
  cxx_library_with_type(${name} "" "${cxx_flags}" ${ARGN})
endfunction()

function(gbench_shared_library name cxx_flags)
  cxx_library_with_type(${name} SHARED "${cxx_flags}" ${ARGN})
endfunction()

function(gbench_library name cxx_flags)
  cxx_library_with_type(${name} "" "${cxx_flags}" ${ARGN})
endfunction()

########################################################################