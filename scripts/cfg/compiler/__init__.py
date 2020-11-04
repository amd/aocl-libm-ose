#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

class Compiler:
    def init_env(self):
        self.env_modifiers = {
            'CC'          : '',
            'CXX'         : '',
            'CPPFLAGS'    : [],
            'CFLAGS'      : [],
            'CXXFLAGS'    : [],
            'CPPDEFINES'  : [],
            'LDFLAGS'     : []
        }

    def CCCmd(self):
        return self.env_modifiers['CC']

    def CXXCmd(self):
        return self.env_modifiers['CXX']

    def CFlags(self):
        return self.env_modifiers['CFLAGS']

    def LDFlags(self):
        return self.env_modifiers['LDFLAGS']

    def Append(self, d):
        for key,value in d.items():
            if isinstance(self.env_modifiers[key], list):
                self.env_modifiers[key].extend(value)
            else:
                self.env_modifiers[key] = value

    def Replace(self, d):
        self.env_modifiers.update(d)

    def __init__(self, prod_mode):
        self.prod_mode = prod_mode
        self.init_env()

        compile_flags_debug = [
            '-g',
            '-Og',
            #'-march=native',
        ]

        compile_flags_release = [
            #'-Ofast',
            #'-march=native',
            #'-fipa-pta',
            #'-funsafe-loop-optimizations',
            #'-flto=4',
            #'-fno-strict-aliasing',
        ]

        link_flags_debug = []
        link_flags_release = compile_flags_release

        cpp_flags_debug = []
        cpp_flags_release = []

        pmode = prod_mode.lower()

        if pmode == "debug" :
            self.Append({
                'CFLAGS'  : compile_flags_debug,
                'LDFLAGS' : link_flags_debug,
                'CPPFLAGS': cpp_flags_debug,}
            )
        elif pmode == "release":
            self.Append({
                    'CFLAGS'  : compile_flags_release,
                    'LDFLAGS' : link_flags_release,
                    'CPPFLAGS': cpp_flags_release,}
            )

