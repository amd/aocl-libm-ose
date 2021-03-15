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

import SCons
import os
from os import mkdir, makedirs, environ
from os.path import join as joinpath, split as splitpath

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

# We need a better name for this
from scripts.cfg import cfg,helper
from scripts.cfg import DefaultCfg
from scripts.cfg.helper import PrintBanner

defcfg = DefaultCfg(build_root=Dir('#build', create=True))

env = defcfg.GetDefaultEnv()

#check intel lib path
intel_lib_path = None
libabi = env['libabi']
if libabi == 'svml':
    for p in env['ENV']['PATH'].split(':'):
        if 'intel' in p:
            intel_lib_path=p
            break

    if intel_lib_path is None:
        print ("Error! Intel lib not found")
        Exit(2)
    else:
        print (intel_lib_path)
        env.Append(INTEL_LIB_PATH = intel_lib_path)

#get include path
inc_path = os.path.join(os.getcwd(), 'include')

# Add shared top-level headers
env.Prepend(CPPPATH=[Dir('include')])

build_root = env['BUILDROOT']

makedirs(build_root, exist_ok=True)

gvars = Variables(defcfg.def_env_file, args=ARGUMENTS)
gvars.AddVariables(
    #('CC', 'C compiler', environ.get('CC', env['CC'])),
    #('CXX', 'C++ compiler', environ.get('CXX', env['CXX'])),
    ('BUILDROOT', 'Build root', environ.get('BUILDROOT', env['BUILDROOT']))
)

# Update env environment with values from ARGUMENTS & global_vars_file
gvars.Update(env)
help_texts = defcfg.GetHelpTexts()
help_texts["global_vars"] += gvars.GenerateHelpText(env)

# These targets are not the .obj files or .o files, instead
# class targets or build objectw
targets = []

libenv = env.Clone()
libenv.Append(
	#INCPATH=['#include'],
    INCPATH=inc_path,
	CWD='#src',
)

# To generate a file version.build.c having current gitversion in VERSION_STRING
libenv.Tool('gitversion')

build_version = libenv.GenerateVersion('src/version.build.h')
libenv.AlwaysBuild(build_version)

amdlibm = SConscript('src/SConscript',
                       exports = { 'env' : libenv },
                       duplicate = 0,
                       variant_dir = joinpath(build_root, 'src'))

targets += amdlibm

#
# Build Test lib and associated tests
#

testenv = env.Clone()
if libabi == 'svml':
    testenv.Append(
	    LIBPATH=['#'+joinpath(build_root,'src'), env['INTEL_LIB_PATH']]
    )
else:
    testenv.Append(
        LIBPATH=['#'+joinpath(build_root,'src')]
    )

test_lib_objs = []  			# Will fill at a later date
test_objs = SConscript(dirs='tests',
                       exports = {'env' : testenv},
                       duplicate = 0,
                       src_dir    = 'tests',
                       must_exist  = 0,
                       variant_dir = joinpath(build_root, 'tests'))

gtest_objs = SConscript(dirs='gtests',
                       exports = {'env' : testenv},
                       duplicate = 0,
                       src_dir    = 'gtests',
                       variant_dir = joinpath(build_root, 'gtests'))

if 'tests' in COMMAND_LINE_TARGETS:
    targets += test_objs

if 'gtests' in COMMAND_LINE_TARGETS:
    targets += gtest_objs

Progress('\r', overwrite=True)
Default(targets)

import atexit
atexit.register(PrintBanner, libenv)

# base help text
Help('''
Usage: scons [scons options] [build variables] [target(s)]

Extra scons options:
%(options)s

Global build variables:
%(global_vars)s

Local Variables:
%(local_vars)s
''' % help_texts)

