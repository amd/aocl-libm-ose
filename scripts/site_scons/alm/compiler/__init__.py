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

import os
#from .gcc import Gcc
#from .llvm import Llvm
#from .intel import Icc

#__all__ = ['Gcc', 'LLVM', 'Icc']

class Compiler:
    def __init__(self, prod_mode, bvars = None, opts = None):
        self.vars = bvars
        self.opts = opts

        self.version_str = '--version'
        self.prod_mode = prod_mode

        self.compile_flags_debug = [
            '-g',
            '-Og',
        ]

        self.compile_flags_release = [
            '-Ofast',
            #'-fipa-pta',
            #'-funsafe-loop-optimizations',
            #'-flto=4',
        ]

        self.compile_flag_map = {
			'debug': self.compile_flags_debug,
			'release' : self.compile_flags_release
		}

        self.link_flags_debug = []

        self.link_flags_release = self.compile_flags_release

        self.link_flag_map = {
            "debug": self.link_flags_debug,
            "release": self.link_flags_release
        }

        self.cpp_flags_debug = []

        self.cpp_flags_release = []

        self.cpp_flag_map = {
            "debug": self.cpp_flags_debug,
            "release": self.cpp_flags_release
        }

        self.ENV = {
            'CC'       : '',
            'CXX'      : '',
            'CFLAGS'   : [],
            'CXXFLAGS' : [],
            'LDFLAGS'  : [],
        }

    def __update_flags(self, flags, new):
        flags += new

    def SetCmd(self, cmd):
        self.ENV['CC'] = cmd

    def SetCxxCmd(self, cmd):
        self.ENV['CXX'] = cmd

    def Cmd(self):
        return self.ENV['CC']

    def CxxCmd(self):
        return self.ENV['CXX']

    def CFlags(self):
        return self.ENV['CFLAGS']

    def LDFlags(self):
        return self.ENV['LDFLAGS']

    def UpdateCFlags(self, flags):
        self.ENV['CFLAGS'] += flags

    def UpdateCXXFlags(self, flags):
        self.ENV['CXXFLAGS'] += flags

    def Setup(self):
        '''
        Setup from environment and update CC,CXX,CFLAGS etc
        '''
        self.__fixup_from_env()

        self.UpdateCFlags(self.cpp_flag_map[self.prod_mode])
        self.UpdateLDFlags(self.link_flag_map[self.prod_mode])

    def Update(self, env):
        '''
        Update from SCons environment variables passed at
        command line, also update the scons environment 'env'
        '''
        self.__fixup_from_vars(env)

        #print('--from compiler-> ', self.Cmd())
        env['CC']        = self.Cmd()
        env['CXX']       = self.CxxCmd()
        env['CFLAGS']    = self.CFlags()
        env['LINKFLAGS'] = self.LDFlags()

    def UpdateLDFlags(self, flags):
        self.ENV['LDFLAGS'] += flags

    def __fixup_from_vars(self, env):
        '''
        Fixing up from VARS, has higher precedence than
        From environment variables
        '''
        if env['CC']:
            self.SetCmd(env['CC'])

        if env['CXX']:
            self.SetCxxCmd(env['CXX'])

        if env['CFLAGS']:
            self.UpdateCFlags(env['CFLAGS'])

        dct = env.Dictionary()
        if 'LINKFLAGS' in dct.keys():
            self.UpdateLDFlags(env['LINKFLAGS'])

    def __fixup_from_env(self):
        """
        """
        if 'CC' in os.environ:
            self.SetCmd(os.getenv('CC'))

        if 'CXX' in os.environ:
            self.SetCxxCmd(os.getenv('CXX'))

        if 'CFLAGS' in os.environ:
            self.UpdateCFlags(os.getenv['CFLAGS'])

        if 'LDFLAGS' in os.environ:
            self.UpdateLDFlags(os.getenv['LDFLAGS'])

    def IsGCC(self):
        return False

    def IsLLVM(self):
        return False

    def IsAOCC(self):
        return False

    def IsICC(self):
        return False

    def Version(self):
        """
        Get the version for each of the compiler type
        """
        import subprocess, re

        process = subprocess.Popen(
            [self.Cmd(), self.version_str], stdout=subprocess.PIPE)
        (stdout, stderr) = process.communicate()

        match = re.search('[0-9][0-9.]*', stdout)
        if match is None:
            return None

        version = match.group().split('.')

        return version
