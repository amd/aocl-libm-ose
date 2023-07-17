#
# Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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
import os
from os import mkdir, makedirs, environ
from os.path import join as joinpath, split as splitpath
import shutil

def AddSiteDir(site_dir):
  """Adds a site directory, as if passed to the --site-dir option.
  Args:
    site_dir: Site directory path to add, relative to the location of the
        SConstruct file.
  This may be called from the SConscript file to add a local site scons
  directory for a project.  This does the following:
     * Adds site_dir/site_scons to sys.path.
     * Imports site_dir/site_init.py.
     * Adds site_dir/site_scons to the SCons tools path.
  """
  # Call the same function that SCons does for the --site-dir option.
  SCons.Script.Main._load_site_scons_dir( site_dir)

AddSiteDir("scripts")

from alm.env import AlmEnvironment
from alm import print_build_failures, print_build_config

__almenv = AlmEnvironment()
__almenv.Setup()
aenv = __almenv.GetDefaultEnv()
if aenv['HOST_OS'] == 'win32':
    aenv['ENV']['TMP'] = os.environ['TMP'] # to avoid linker eror in windows. This is mentioned in scons FQA doc.
    shutil.copy(r'.\scripts\libalm.def', r'.\src')
    shutil.copy(r'.\scripts\almfast.def', r'.\src\fast')
    shutil.copy(r'.\scripts\mparith32.def', r'.\gtests\libs\mparith')
    shutil.copy(r'.\scripts\mparith64.def', r'.\gtests\libs\mparith')

# First check version of python and scons
EnsurePythonVersion(3, 6)
EnsureSConsVersion(3, 1, 1)

# Register exit printers
import atexit
atexit.register(print_build_config, aenv)
atexit.register(print_build_failures)

# include path
inc_path = '#include'

# top level src path
src_path = '#src'

# Add shared top-level headers
aenv.Prepend(CPPPATH=[Dir('include')])

aenv['BUILDROOT'] = aenv['BUILDDIR']

build_root = aenv['BUILDROOT']

makedirs(build_root, exist_ok=True)

# alm libs will be generated here
alm_lib_path='#'+joinpath(build_root,'src')

# Fetch absolute path of libaoclutils library
aocl_utils_install_path = joinpath('#', aenv['aocl_utils_install_path'])
Export('aocl_utils_install_path')

# These targets are not the .obj files or .o files, instead
# class targets or build objectw
targets = []

libenv = aenv.Clone()
libenv.Append(
        INCPATH=inc_path,
        CWD=src_path,
)

# To generate a file version.build.c having current gitversion in VERSION_STRING
libenv.Tool('gitversion')

build_version = libenv.GenerateVersion('src/version.build.h')
libenv.AlwaysBuild(build_version)

build_sys_details = libenv.GetBuildInfo('src/buildsysinfo.h')
libenv.AlwaysBuild(build_sys_details)

alm_libs = SConscript(joinpath(src_path, 'SConscript'),
                     exports = { 'env' : libenv },
                     duplicate = 0,
                     variant_dir = alm_lib_path)

targets += alm_libs

gtest_objs = []
if 'tests' in COMMAND_LINE_TARGETS or ('gtests' in COMMAND_LINE_TARGETS) :
    if aenv['HOST_OS'] == 'win32':
        #initialize different library paths
        mpfr_inc = Dir('..\mpfr\mpfr_x64-windows\include')
        gmp_inc = Dir('..\mpfr\gmp_x64-windows\include')
        mpc_inc = Dir('..\mpfr\mpc_x64-windows\include')
        mpfr_lib = Dir('..\mpfr\mpfr_x64-windows\lib')
        gmp_lib = Dir('..\mpfr\gmp_x64-windows\lib')
        mpc_lib = Dir('..\mpfr\mpc_x64-windows\lib')

        aenv['ENV']['INCLUDE'] = [aenv['ENV']['INCLUDE'], mpfr_inc, gmp_inc, mpc_inc]
        aenv.Append(LIBS=['gmp.lib', 'mpfr.lib', 'mpc.lib'],LIBPATH=[mpfr_lib, gmp_lib, mpc_lib])

    testenv = aenv.Clone()

    LIBPATH=[alm_lib_path]

    gtest_objs += SConscript(dirs='gtests',
                   exports = {'env' : testenv, 'almenv': __almenv},
                   duplicate = 0,
                   src_dir    = 'gtests',
                   variant_dir = joinpath(build_root, 'gtests'))

if gtest_objs:
    Requires(gtest_objs, alm_libs)
    targets += gtest_objs

Default(targets)

