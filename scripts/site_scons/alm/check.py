#
# Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
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
from os.path import join as joinpath

import SCons
from SCons.Script import Configure, GetOption, COMMAND_LINE_TARGETS
from SCons.Script import Exit
from SCons.SConf import *

from os.path import join as joinpath

toolchain_versions = {
    #Toolchain : {preferred_version, min_version}
    'GCC' :     {'max':'14.1' ,  'min':'9.2'},
    'CLANG':    {'max':'18.1',   'min':'9.0'},
    'AOCC':     {'max':'18.1',   'min':'9.0'},
    'MSVC':     {'max':'12.0',   'min':'2.2'},
}

toolchain_macros = {
    "GCC"  : 'defined(__GNUC__) && !defined(__clang__)',
    "AOCC" : 'defined(__clang__)',
    "MSVC" : 'defined(_MSC_VER)',
}

def CheckForProgram(context):
    """
    Check if a program exist in the PATH
    """
    env = context.env
    conf = env.conf
    result = conf.CheckProg(env.compiler.Cmd())
    context.Result(result)

def CheckProgramVersion(context, pgm, version):
    context.Message('Checking for Program...', pgm)
    result = conf.TryLink(mylib_test_source_file, '.c')
    context.Result(result)

def CheckForToolchain(context):
    context.Message('Checking for Toolchain ')
    result = False
    env = context.env
    cc = env['compiler']
    cc_ver = env['CCVERSION']
    for k,v in toolchain_versions.items():
        if k.lower() in cc:
            #ignoring last . (if exists) to convert to float
            if cc_ver.count('.') > 1:
                cc_ver_minor = cc_ver[cc_ver.rfind(".")+1:]
                cc_ver = cc_ver[:cc_ver.rfind(".")]
            context.Message(' Using compiler {0} version {1} minor version {2}'.format(k, cc_ver, cc_ver_minor))
            if float(v['min']) <= float(cc_ver) <= float(v['max']):
                    result = True

    context.Result(result)
    return result

def CheckPathDir(context, mydir):
    from os.path import isdir
    from os import listdir

    if not isdir(mydir) or len(os.listdir(mydir)) == 0:
        print ("Invalid/Empty directory: %s" % mydir)
        return False
    return True

def CheckCPUIDInstall(context):
    res = False
    env = context.env
    aocl_utils_install_path = env['aocl_utils_install_path']

    context.Message ("Checking for valid AOCL UTILS install path")

    # check if path exists, else exit error
    if CheckPathDir(context, aocl_utils_install_path):
        res = True
    context.Result(res)
    return res

def CheckLibAbi(context):
    # If ABI is SVML, check for OneAPI library path in INTEL_LIB_PATH variable
    # Not using CheckLibWithHeader because this might be
    # in user defined local paths, not under /usr/lib.
    res = False
    env = context.env
    libabi = (env['libabi'])

    context.Message("Checking if test supporting libraries exists")

    if libabi == 'svml':
        context.Message('INTEL_LIB_PATH')
        oneapi_path = os.environ.get('INTEL_LIB_PATH', None)
        if oneapi_path is None or '':
            context.Message('\nThe env variable INTEL_LIB_PATH is not set\n')
            exit (1)

        if CheckPathDir(context, oneapi_path):
            env['INTEL_LIB_PATH'] = oneapi_path
            context.Message(oneapi_path)
            res = True
        else:
            context.Message("INTEL_LIB_PATH not found\n")

    # TODO: Add for other lib ABI later
    else:
        res = True

    context.Result(res)

def CheckForOS(context):
    pass

def CheckCompilerFlag(context, flag):
    candidate = """
    #include <stdio.h>
    int main() { printf("hello world"); return 0; }
    """
    oldcflags = context.env['CFLAGS']
    context.env['CFLAGS'] = flag
    context.Message("Checking if [%s] is supported by %s "%(flag,
                                                            context.env['CC']))
    result = context.TryCompile(candidate, '.c')
    context.Result(result)
    context.env['CFLAGS'] = oldcflags

    return result

def All(almenv):
    """
    """
    env = almenv.env

    def CheckZenVer(ctx):
        for f in ['znver5', 'znver4', 'znver3', 'znver2', 'znver1']:
            ret = CheckCompilerFlag(ctx, '-march='+f)
            if ret :
                ctx.env['ALM_MAX_ARCH'] = f
                return ret

        ctx.Message("Unable to detect compiler support for Zen architecture\n")
        ctx.env['ALM_MAX_ARCH'] = 'x86_64'
        return None

    conf = env.Configure (
        help = False,
        custom_tests = {
            'CheckForToolchain' : CheckForToolchain,
            'CheckForOS'        : CheckForOS,
            'CheckLibAbi'       : CheckLibAbi,
            'CheckCPUIDInstall' :   CheckCPUIDInstall,
            'CheckZenVer'       : lambda ctx : CheckZenVer(ctx),
        },
        conf_dir = joinpath(env['BUILDDIR'], '.sconf_temp'),
    )

    result = conf.CheckProg(almenv.compiler.Cmd())
    result = conf.CheckProg(almenv.compiler.CxxCmd())

    #print('->>',  almenv.compiler.Cmd())
    result = conf.CheckProg(almenv.compiler.Cmd())
    result = conf.CheckProg(almenv.compiler.CxxCmd())

    if not conf.CheckForToolchain():
        print ('Unsupported compiler version')
        print ('Supported versions:')
        for k,v in toolchain_versions.items():
            print (k + ' min: ' + v['min'] + ' max ' + v['max'])
        Exit(1)

    if conf.CheckLibAbi():
        Exit(1)

    conf.CheckZenVer()

    if not conf.CheckCPUIDInstall():
        Exit(1)

    return conf

class AlmChecker(object):
    def __init__(self, almenv):
        self.almenv  = almenv
        self.conf    = None

    def Start(self):
        # Dont Configure while cleaning
        almenv = self.almenv

        if 'clean' in COMMAND_LINE_TARGETS:
            return

        # We are now in a stage, just before building
        # Check for tools, compilers, headers and libraries
        self.conf = All(almenv)

        almenv.conf  = self.conf

    def Finish(self):
        self.env = self.conf.Finish()
