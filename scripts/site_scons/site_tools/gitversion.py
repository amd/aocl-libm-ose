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


def generate(env):
    env.AddMethod(generate_version, 'GenerateVersion')


def exists(env):
    return True

