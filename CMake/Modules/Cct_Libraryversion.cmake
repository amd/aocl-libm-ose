#
# Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

# Function to extract version from alm_version.h header file
function(extract_alm_version_from_header header_file output_var)
    if(NOT EXISTS "${header_file}")
        message(FATAL_ERROR "Version header file not found: ${header_file}")
    endif()

    # Read the header file
    file(READ "${header_file}" header_content)

    # Primary regex - matches: ALM_VERSION_STRING[] = "5.0.1";
    string(REGEX MATCH "ALM_VERSION_STRING\\[\\][ \t]*=[ \t]*\"([^\"]+)\"" version_match "${header_content}")

    if(CMAKE_MATCH_1)
        set(${output_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
        message(STATUS "Extracted ALM version from header: ${CMAKE_MATCH_1}")
        return()
    endif()

    # Alternative regex - matches any version pattern like "x.y.z"
    string(REGEX MATCH "\"([0-9]+\\.[0-9]+\\.[0-9]+[^\"]*)\"" fallback_match "${header_content}")
    if(CMAKE_MATCH_1)
        set(${output_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
        message(STATUS "Extracted ALM version using fallback pattern: ${CMAKE_MATCH_1}")
        return()
    endif()

    # If all patterns fail, provide detailed error
    message(FATAL_ERROR
        "Could not extract ALM_VERSION_STRING from ${header_file}.\n"
        "Expected format: static const char ALM_VERSION_STRING[] = \"x.y.z\";\n"
        "Please check the file format and ensure it contains the version string.")
endfunction()

# Function to validate version format
function(validate_version_format version_string)
    string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+.*$" valid_format "${version_string}")
    if(NOT valid_format)
        message(FATAL_ERROR "Invalid version format: ${version_string}. Expected format: x.y.z")
    endif()
endfunction()
