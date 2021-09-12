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

import subprocess
import sys

import SCons.Action

__gitversion = None

def get_git_version(env):
    if env.GetOption('help') or env.GetOption('clean'):
        return

    global __gitversion

    if __gitversion:
        return __gitversion

    try:
        #if not conf.CheckProg('git'):
        #    __gitversion = 'unknown'
        #    return

        kw = { 'stdin' : 'devnull',
               'stdout' : subprocess.PIPE,
               'stderr' : subprocess.PIPE,
               'universal_newlines' : True,
        }
        p = SCons.Action._subproc(env, ['git', 'describe', '--dirty= (modified)', '--always'], **kw)
        out,err = p.communicate()
        status = p.wait()
        if err:
            sys.stderr.write(unicode(err))

        if status:
            __gitversion = 'unknown'
        else:
            __gitversion = out.strip()
    finally:
        return __gitversion


# AMD Libm version template

__amd_libm_version_template="""/*
* This file is automatically generated by the build process
* DO NOT EDIT!
*/

static const char VERSION_STRING[] = "%s";

static const char* alm_get_build(void);

static const char* alm_get_build(void)
{
        return VERSION_STRING;
}
"""

def generate_version(env, target):
    """Generate the version file with the current version in it"""
    #print("generate_version", "target:", target, "env:", env)

    version = get_git_version(env)

    contents = __amd_libm_version_template % (version)

    fd = open(target, 'w')
    fd.write(contents)
    fd.close()

    return 0

# include build system details into version information
__amd_libm_build_info_template="""/*
* This file is automatically generated during build
*/

static const char build_sys_info[] = "%s";

"""

# Exit with displaying error message
def Error_Exit(msg):
    print ('Error: ' + msg)
    exit(1)

#run any shell command, return op, err, err code
def RunCommand(cmd):
    import subprocess
    print ('Running cmd: '+cmd)
    p = subprocess.Popen(cmd, shell = True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    out=str(out, 'utf-8')
    err=str(err, 'utf-8')
    if err.strip() == '':   err=None
    if out.strip() == '':   out=None
    #if error, just exit with err code
    if p.returncode != 0:
        Error_Exit ('Proc failed with retcode:\n' +
                     str(p.returncode) +
                    '\nError:' + str(err))
    return out, err, p.returncode

# get aocc release version to be included in the version string
def GetAOCCVersion(compiler):
    aocc_ver=' '
    lines = RunCommand('{0} --version'.format(compiler))[0]
    for l in lines.split('\n'):
        if 'AMD clang version' in l and 'CLANG:' in l:
            aocc_ver = [v for v in l.split() if 'AOCC_' in v][0]
    return aocc_ver

def GetBuildInfo(env, target):
    """generate a file with build system info in it"""
    cc = env['CC']
    import ntpath
    import os
    # get compiler exe file from full path
    # we donot need full path string in the version
    if os.path.exists(os.path.dirname(cc)):
        cc = ntpath.basename(cc)
    cc_ver = env['CCVERSION']

    # if clang, fetch aocc release version too
    if 'clang' in cc:
        cc_ver += '-' + GetAOCCVersion(cc)

    import platform
    platform = platform.platform()
    sys_details = '%s-%s-%s' % (cc, cc_ver, str(platform))
    contents = __amd_libm_build_info_template % (sys_details)
    f = open(target, 'w')
    f.write(contents)
    f.close()
    return 0

def generate(env):
    env.AddMethod(generate_version, 'GenerateVersion')
    env.AddMethod(GetBuildInfo, 'GetBuildInfo')

def exists(env):
    return True

