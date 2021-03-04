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


import SCons
from SCons.Script import Configure, GetOption, COMMAND_LINE_TARGETS
from SCons.Script import Exit
from SCons.SConf import *

toolchain_versions = {
    #Toolchain : {preferred_version, min_version}
    'GCC' :     {'max':'10.2' ,  'min':'9.2'},
    'CLANG':    {'max':'12.0',   'min':'9.0'},
    'MSVC':     {'max':'12.0',   'min':'2.2'},
    'ICC' :     {'max':'2020.2', 'min':'2020.1'},  # Is this really correct ?
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
    from SCons.SConf import CheckCC, CheckCXX
    result = False
    env = context.env
    context.Message('Checking for Tool Chain')
    if not CheckCC(context) or CheckCXX(context):
        cc,cxx = env['CC'], env['CXX']
        cc_ver, cxx_ver = env['CCVERSION'], env['CXXVERSION']
        for k,v in toolchain_versions.items():
            if k.lower() in cc:
                print ('Using compiler {0} ver {1}'.format(k, cc_ver))
                #ignoring last . to convert to float
                cc_ver = '.'.join(cc_ver.split('.')[:-1])
                cxx_ver = '.'.join(cxx_ver.split('.')[:-1])
                if float(v['min']) <= float(cc_ver) <= float(v['max']) and \
                    float(v['min']) <= float(cxx_ver) <= float(v['max']):
                    result = True
    context.Result(result)
    return result

def CheckForHeaders(context):
    """
    """
    res = CheckCHeader(context, 'mpfr.h')
    if not res:
        print('Did not find mpfr.h')

    context.Result(res)

def CheckForLibs(context):
    """
    """
    res = CheckLibWithHeader(context,
                   'mpfr',
                   header='mpfr.h',
                   language='c')

    if not res:
        print('Did not find libmpfr, AOCL LibM tests wont work!')
        context.Result(res)

def CheckLibAbi(context):
    res = False
    #if svml, check for svml path in INTEL_LIB_PATH variable
    #not using CheckLisWithHeader because this might be
    #in user defined local paths, not under /usr/lib
    env = context.env
    intel_lib_path = None
    libabi = (env['libabi'])

    if libabi == 'svml':
        context.Message ("Checking for Intel SVML Lib path\n")
        #get val of shell variable INTEL_LIB_PATH
        if os.environ.get('INTEL_LIB_PATH', None) is None:
            print("Environment variable INTEL_LIB_PATH not found")
        else:
            #add to env INTEL_LIB_PATH
            env['INTEL_LIB_PATH'] = os.environ.get('INTEL_LIB_PATH')
            #check if that path is valid and not empty
            if not os.path.isdir(env['INTEL_LIB_PATH']) or len(os.listdir(env['INTEL_LIB_PATH'])) == 0:
                print ("Invalid/empty Intel SVML path")
            else:
                context.Message("Using Intel lib path " + env['INTEL_LIB_PATH'])
                res = True

    else:   #will add other libabis later
        res = True

    context.Result(res)

def CheckForOS(context):
    pass

def All(almenv):
    """
    """
    env = almenv.env

    conf = env.Configure(help = False, custom_tests = {
        'CheckForToolchain' : CheckForToolchain,
        'CheckForOS'        : CheckForOS,
        'CheckForHeaders'   : CheckForHeaders,
        'CheckForLibs'      : CheckForLibs,
        'CheckLibAbi'       : CheckLibAbi,
    })

    if conf.CheckLibAbi():
        Exit(1)

    if not conf.CheckForToolchain():
        print ('Unsupported compiler version')
        print ('Supported versions:')
        for k,v in toolchain_versions.items():
            print (k + ' min: ' + v['min'] + ' max ' + v['max'])
        Exit(1)

    #result = conf.CheckProg(almenv.compiler.CxxCmd())

    # if conf.CheckForToolchain():
    #     print("Toolchain not found")
    #     Exit(1)

    # if conf.CheckForHeaders():
    #     print("Headers not found")
    #     Exit(1)

    # if conf.CheckForLibs():
    #     print("Support Libraries not found")
    #     Exit(1)

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
