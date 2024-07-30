#
# Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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

#
# Set Package version extracted from a string of format
#   ${major}.${minor}.${patch}-${patch_extra}
#
function(cct_extract_version_details var_prefix version_string)

set(AU_VERSION_STRING_REGEX "^([0-9]+)\\.([0-9]+)\\.([0-9]+)([-])(rc|dev|\\.)?([0-9]*)$")
string(REGEX MATCH ${AU_VERSION_STRING_REGEX} version_matches "${version_string}")

# don't use CMAKE_MATCH_0 for `full` since it may not have the `v` prefix.
if(CMAKE_MATCH_0)
    set(major ${CMAKE_MATCH_1})
    set(minor ${CMAKE_MATCH_2})
    set(patch ${CMAKE_MATCH_3})
    set(suffix ${CMAKE_MATCH_5})
    set(suffix_extra ${CMAKE_MATCH_6})
    set(full "${major}.${minor}.${patch}${suffix}${suffix_extra}")

    set(${var_prefix} "${major}.${minor}.${patch}" PARENT_SCOPE)
    set(${var_prefix}_FULL ${full} PARENT_SCOPE)
    set(${var_prefix}_MAJOR ${major} PARENT_SCOPE)
    set(${var_prefix}_MINOR ${minor} PARENT_SCOPE)
    set(${var_prefix}_PATCH ${patch} PARENT_SCOPE)
    set(${var_prefix}_SUFFIX_EXTRA ${suffix_extra} PARENT_SCOPE)

if(AU_CMAKE_VERBOSE)
	message("version string: " ${full})
	message("version major: " ${major})
	message("version minor: " ${minor})
	message("version patch: " ${patch})
	message("version suffix: " ${suffix})
	message("version suffix-extra: " ${suffix_extra})
endif()

endif()
endfunction()

function(dummy_dont_call)
set(AU_VERSION_STRING_REGEX "^([0-9]+)\\.([0-9]+)\\.([0-9]+)([-]rc|\\.)?([0-9]*)$")
string(REGEX REPLACE "${AU_VERSION_STRING_REGEX}" "\\1"
        AU_VERSION_MAJOR "${AU_VERSION_STRING_PRE}")
string(REGEX REPLACE "${AU_VERSION_STRING_REGEX}" "\\2"
        AU_VERSION_MINOR "${AU_VERSION_STRING_PRE}")
string(REGEX REPLACE "${AU_VERSION_STRING_REGEX}" "\\3"
        AU_VERSION_PATCH "${AU_VERSION_STRING_PRE}")
string(REGEX REPLACE "${AU_VERSION_STRING_REGEX}" "\\5"
        AU_VERSION_PRERELEASE "${AU_VERSION_STRING_PRE}")
endfunction()

