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

from . import Compiler

class LLVM(Compiler):
    def __init__(self, prod_mode, bvars = None, opts = None):
        super(LLVM, self).__init__(prod_mode, bvars, opts)
        self.cmd = 'clang'
        self.cxxcmd = 'clang++'
        self.compile_flags_debug = [
            '-g',
            '-Og',
            '-march=native',
        ]
        self.compile_flags_release = [
            '-ffp-contract=fast', # Needed to generate FMA instructions for vector routines
            #'-Ofast',
            # '-march=native',
            # '-fipa-pta',
            # '-funsafe-loop-optimizations',
            # '-flto=4',
        ]
        self.compile_flag_map = {
            'debug': self.compile_flags_debug,
            'release' : self.compile_flags_release
        }

        self.link_flags_debug = ['-fuse-ld=ld']
        self.link_flags_release = ['-fuse-ld=ld']

        self.link_flag_map = {
            "debug": self.link_flags_debug,
            "release": self.link_flags_release
        }

        self.cpp_flag_map = {
            "debug":   self.cpp_flags_debug,
            "release": self.cpp_flags_release
        }

        self.cpp_flags_debug = []
        self.cpp_flags_release = []

        self.warnings = [
            '-Weverything',
        ]

        self.fixup_from_env()

    def Cmd(self):
        return self.cmd

    def CxxCmd(self):
        return self.cxxcmd

    def CFlags(self):
        return self.compile_flag_map[self.prod_mode]

    def LDFlags(self):
        return self.link_flag_map[self.prod_mode]
