#
# Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from SCons.Variables import Variables
from SCons.Script import ARGUMENTS, PathVariable, GetOption

import platform

def variable_shlex_splitter(val):
    parse_mode = 'other' if platform.is_running_os('windows') else 'posix'
    return shlex.split(val, posix=(parse_mode == 'posix'))

class AlmVariables(Variables):
    def __init__(self):
        self.variables_files = ['global.vars']

        super().__init__(files=self.variables_files,
                         args = ARGUMENTS)

        self.AddVariables(
            PathVariable('AR', 'Set path for Archiver', None),
            PathVariable('AS', 'Set path for Assembler', None),
            PathVariable('CC', 'Set path for C Compiler', None),
            PathVariable('CXX', 'Set path for C++ Compiler', None),
            PathVariable('FC', 'Set path for Fortran Compiler', None),
            PathVariable('LD', 'Set path for Linker', None),
        )

        self.Add('ARFLAGS',
                 help = 'Set Archiver flags',
                 converter=variable_shlex_splitter
        )

        self.Add('ASFLAGS',
                 help = 'Set Assembler flags',
                 converter=variable_shlex_splitter
        )

        self.Add('CCFLAGS',
                 help = 'Set C Compiler flags',
                 converter=variable_shlex_splitter
        )

        self.Add('CPPFLAGS',
                 help = 'Set C Preprocessor flags',
                 converter=variable_shlex_splitter
        )

        self.Add('CXXFLAGS',
                 help = 'Set C++ Compiler flags',
                 converter=variable_shlex_splitter
        )

        self.Add('FCFLAGS',
                 help = 'Set Fortran Compiler flags',
                 converter=variable_shlex_splitter
        )

        self.Add('LDFLAGS',
                 help = 'Set Linker flags',
                 converter=variable_shlex_splitter
        )

        self.Add('DEVELOPER',
                 help        = 'Set the Developer mode',
                 default     = 0,
                 ignore_case = 0,
        )
