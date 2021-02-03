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

from os import environ
from os.path import join as joinpath

from SCons.Node.FS import Dir
from SCons.Script import Environment, COMMAND_LINE_TARGETS, Exit

from . import compiler
from .compiler import gcc, icc, llvm
from .compiler import helper as compile_helper
from .variables import AlmVariables
from .options import AlmOptions
from .check import AlmChecker

installdirs = {
    'root'    : '.',
    'docs'    : 'docs/',
    'lib'     : 'lib/',
    'bin'     : 'bin/',
    'include' : 'include/'
}

def get_environ_path():
    path = environ['PATH']

    path = path.replace(' ', '\\ ')

    #print(path)
    return path

class AlmEnvironment(object):
    def __init__(self):
        self.builddir  = None
        self.envDict   = None
        self.conf      = None
        self.compiler_type = None
        self.build     = None
        self.compiler  = None

        self.envDict = dict (
            BUILD         = '',
            BUILDDIR      = '',
            PATH          = get_environ_path(),
            TOOLCHAIN_DIR = '',
            DEVELOPER     = '',
            COMPILER      = '',
            #LDFLAGS       = '',
        )

        self.env = Environment(
            variables   = AlmVariables(),
            installdirs = installdirs,
            ENV         = self.envDict,
        )

    def Setup(self):
        """
        """
        aopts = AlmOptions(self.env)
        aopts.Setup()

        self.opts    = aopts

    def __configure_compiler(self):
        # Configure Compiler

        buildtype = self.env['BUILD']
        cc_opt = self.compiler_type  = self.opts.GetOption('compiler')

        cc_env = self.env['CC']
        if cc_env.endswith('clang'):
            print(
                'Found LLVM compiler in CC variable, Setting up correctly..'
            )
            cc_opt = 'llvm'

        if cc_opt == 'gcc':
            self.compiler = gcc.Gcc(buildtype)
            self.env['compiler'] = 'gcc'
        elif cc_opt in ['aocc', 'llvm']:
            self.compiler = llvm.LLVM(buildtype)
            self.env['compiler'] = 'aocc'
        elif cc_opt == 'icc':
            self.env['compiler'] = 'icc'
            self.compiler = icc.Icc(buildtype)

        self.compiler.Setup()

    def __configure_builddir(self):
        """
        Figure out build and install dirs
        """
        opts                  = self.opts
        self.builddir         = opts.GetOption('build-dir')

        debug = opts.GetOption('debug_mode')
        dev   = opts.GetOption('developer')
        abi   = opts.GetOption('libabi')

        # fix the debug, it is set to 'none' instead of None
        # due to the way options are handled
        debug = None if debug == 'none' else debug

        if debug and dev:
            print("Debug and Developer options cannot be together", debug, dev)
            Exit(1)

        build = self.env['BUILD']
        self.env['developer'] = 0

        dirsuffix = abi

        if not debug and not dev:
            dirsuffix += '-release'
            bld = 'release'
        elif debug:
            dirsuffix += '-debug'
            bld = 'debug'
        elif dev:
            dirsuffix += '-dev'
            bld = 'dev'
            self.env['developer'] = 1
        else:
            print('Unknown build mode')
            bld = 'UNKNOWN'
            #dirsuffix = self.compiler_type

        self.env['BUILD'] = bld
        self.builddir = joinpath(self.builddir, dirsuffix)

        if not self.env['ENV']['BUILDDIR']:
            self.env['ENV']['BUILDDIR']  = self.builddir
            self.env['BUILDDIR'] = self.builddir

        # Setup cachedir
        self.env.CacheDir(joinpath(self.builddir, 'cache'))

        # Setup installdir
        if 'install' in COMMAND_LINE_TARGETS:
            installroot = self.opts.GetOption('prefix')
            for k in installdirs.keys():
                installdirs[k] = joinpath(installroot, installdirs[k])

        self.env['installdirs'] = installdirs


    def __configure_cpp_defines(self):
        """
        Based on the command line options
        build C Preprocessor defines
        """
        env        = self.env
        opts       = self.opts
        abi        = opts.GetOption('libabi')

        abi_dict = {
            'acml' : 'LIBABI_ACML',
            'libm' : 'LIBABI_LIBM',
            'svml' : 'LIBABI_SVML',
            'glibc': 'LIBABI_GLIBC',
        }

        cflags = ''
        try:
            cflags = abidict[abi]
        except:
            cflags = ''

        env.Append (
            CFLAGS = cflags,
        )

        env['libabi'] = abi

    def CheckDefault(self):
        '''
        Check for default programs, headers and libraries
        '''
        self.check.Start()
        self.check.Finish()

    def GetDefaultEnv(self):
        """
        """
        # Type of build, release/debug
        self.env['BUILD'] = self.opts.GetOption('build')

        self.__configure_compiler()

        self.__configure_cpp_defines()

        self.compiler.Update(self.env)

        self.__configure_builddir()

        self.check = AlmChecker(self)

        self.CheckDefault()

        # Setup what is printed while compiling
        compile_helper.UpdateEnvComStr(self.env)

        return self.env


