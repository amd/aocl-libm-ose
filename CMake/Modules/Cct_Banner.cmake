#
# Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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

function(cct_display_banner)

    message("===============================================================")
    message("    Project \"${PROJECT_NAME}\"    ")
    message("===============================================================")
    message("Global Settings:")
    message(" - Project name:          \"${PROJECT_NAME}\"")
    message(" - Project prefix:        \"${PROJECT_PREFIX}\"")
    message(" - Project description:   \"${${PROJECT_PREFIX}_PROJ_DESCRIPTION}\"")
    message(" - Project author:        \"${${PROJECT_PREFIX}_VENDOR_NAME}\"")
    message(" - Author contact:        \"${${PROJECT_PREFIX}_VENDOR_CONTACT}\"")
    message(" - Version Major:         \"${${PROJECT_PREFIX}_VERSION_MAJOR}\"")
    message(" - Version Minor:         \"${${PROJECT_PREFIX}_VERSION_MINOR}\"")
    message(" - Version Patch:         \"${${PROJECT_PREFIX}_VERSION_PATCH}\"")
    message(" - Build Generator:       \"${CMAKE_GENERATOR}\"")
    message("Base Settings:")
    message(" - Build standard version: \"${CMAKE_CXX_STANDARD}\"")
    message(" - Build type:             \"${CMAKE_BUILD_TYPE}\"")
    message(" - Main binary Target:     \"${CMAKE_BINARY_DIR}\"")
    message(" - Toolchain file used:    \"${CMAKE_TOOLCHAIN_FILE}\"")
    message("Test settings:")
    message(" - Enabled? :              \"${${PROJECT_PREFIX}_OPTION_BUILD_TESTS}\"")
    message(" - Enable Slow Tests:      \"${${PROJECT_PREFIX}_OPTION_ENABLE_SLOW_TESTS}\"")
    message(" - Enable Broken Tests:    \"${${PROJECT_PREFIX}_OPTION_ENABLE_BROKEN_TESTS}\"")
    message("Code Coverage Settings:")
    message(" - Enabled? :              \"${${PROJECT_PREFIX}_OPTION_ENABLE_COVERAGE}\"")
    message("Doc Settings:")
    message(" - Enable?:                \"${${PROJECT_PREFIX}_OPTION_BUILD_DOCS}\"")
    message("Export Settings:")
    message(" - Enable?:                \"${${PROJECT_PREFIX}_OPTION_ENABLE_EXPORT}\"")
    message(" - Export namespace:       \"${${PROJECT_PREFIX}_OPTION_EXPORT_NAMESPACE}\"")
    message(" - Install directory:      \"${CMAKE_INSTALL_PREFIX}\"")
    message("Package Settings:")
    message(" - Package:                \"${CPACK_PACKAGING_INSTALL_PREFIX}\"")

endfunction(cct_display_banner)

