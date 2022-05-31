#
# Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
from os.path import abspath

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

    return path

# spack related
def append_spack_env(mydict):
    mydict = {}

    # spack related env variables to be added, if set.
    spack_params = ['SPACK_ENV_PATH',
                    'SPACK_DEBUG_LOG_DIR',
                    'SPACK_DEBUG_LOG_ID',
                    'SPACK_COMPILER_SPEC',
                    'SPACK_CC_RPATH_ARG',
                    'SPACK_CXX_RPATH_ARG',
                    'SPACK_F77_RPATH_ARG',
                    'SPACK_FC_RPATH_ARG',
                    'SPACK_TARGET_ARGS',
                    'SPACK_DTAGS_TO_ADD',
                    'SPACK_DTAGS_TO_STRIP',
                    'SPACK_LINKER_ARG',
                    'SPACK_SHORT_SPEC',
                    'SPACK_SYSTEM_DIRS',
                ]

    # add these keys to mydict if they exist in environ
    for param in spack_params:
        if param in environ:
            mydict[param] = environ[param]

    return mydict

class AlmEnvironment(object):
    def __init__(self):
        self.builddir  = None
        self.envDict   = None
        self.conf      = None
        self.compiler_type = None
        self.build     = None
        self.compiler  = None
        self.arch_config = None
        self.spackenv = None
        self.spackbuildenv = None

        self.envDict = dict (
            BUILD         = '',
            BUILDDIR      = '',
            PATH          = get_environ_path(),
            TOOLCHAIN_DIR = '',
            DEVELOPER     = '',
            COMPILER      = '',
            #LDFLAGS       = '',
        )

        # spack related
        self.spackenv = append_spack_env(self.spackenv)
        self.spackbuildenv = {**self.spackenv, **self.envDict}
        self.envDict = self.spackbuildenv

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
        try:
            cc_env = self.env['ALM_CC']
            if self.env['HOST_OS'] == 'win32':
                self.env['AS'] = self.env['ALM_CC']
                self.env['LINK'] = 'lld-link'
        except KeyError as k:
            cc_env = self.env['CC']

        if 'clang' in cc_env:
            print('Found LLVM compiler in CC variable')
            cc_opt = 'llvm'
            self.compiler = llvm.LLVM(buildtype)
            self.env['compiler'] = 'aocc'

        elif 'gcc' in cc_env:
            print ('Found GCC Compiler in CC variable')
            cc_opt = 'gcc'
            self.compiler = gcc.Gcc(buildtype)
            self.env['compiler'] = 'gcc'

        elif 'icc' in cc_env:
            print ('Found ICC Compiler in CC variable')
            cc_opt = 'icc'
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
        arch_config  = opts.GetOption('arch_config')

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

            # get absolute path
            installroot = abspath(installroot)
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
        arch_config = opts.GetOption('arch_config')

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
        env['arch_config'] = arch_config

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
        builddict = {
                'all' : 'debug',
                'libs': 'debug',
                'tests': 'release',
                'none': 'release'
                }

        self.env['BUILD'] = builddict[self.opts.GetOption('debug_mode')]

        self.__configure_compiler()

        self.__configure_cpp_defines()

        self.compiler.Update(self.env)

        self.__configure_builddir()

        self.check = AlmChecker(self)

        self.CheckDefault()

        # Setup what is printed while compiling
        compile_helper.UpdateEnvComStr(self.env)

        return self.env


