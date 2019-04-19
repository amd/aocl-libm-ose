#
# Copyright (C) 2019 Prem Mallappa.
#
#
# Author: Prem Mallappa<prem.mallappa@gmail.com>
#
import os
from os import mkdir, makedirs, environ
from os.path import join as joinpath, split as splitpath

# We need a better name for this
from scripts.cfg import cfg,helper

build_root  = '#build'
envfile     = 'scripts/helper/env.py'

help_texts = {
    "global_vars": "",
    "options"    : '',
    'local_vars' : ''
}

localopts = cfg.LocalOption(help_texts)

localopts.Add('--verbose', dest='verbose', nargs=1, action='store',
             help='Print full tool command lines')
localopts.Add('--enable-debug', dest='debug', action='store_false',
             help='Enable Debug mode [default:%default]')
localopts.Add('--with-libabi', dest='libabi', nargs=1, action='store',
             choices=['aocc', 'gcc', 'libm', 'svml'],
             help='Compile libs with respective abi(API) calls')
localopts.Add('--with-testabi', dest='testabi', nargs=1, action='store',
             choices=['aocc', 'gcc', 'libm', 'svml'],
             help='Compile tests to call this abi')
localopts.Add('--developer', dest='developer', nargs=1, action='store',
             choices=['1', '2', '3', '4'],
             help='Enable Developer mode')
localopts.Add('--build', dest='build', nargs=1, action='store',
             choices=['release', 'developer', 'debug'],
             help='Enable build type')

AddMethod(Environment, helper.SetupConfiguration)

vars = Variables(envfile)
vars.AddVariables(
    BoolVariable('warnings', 'compilation with -Wall and similiar', 1),
    BoolVariable('shared',   'flag to enable shared libraries', 1),
    BoolVariable('verbose',  'Increase verbosity', 0),
    EnumVariable('debug',    'debug output and symbols', 'no',
                 allowed_values=('no','libs', 'tests', 'all'),
                 map={}, ignorecase=0),  # case sensitive

    EnumVariable('libabi', 'Library function naming', 'aocl',
                 allowed_values=('aocl', 'glibc', 'acml'),
                 map={}, ignorecase=2),  # lowercase always

	# test abi makes tests to call out for given library call
    EnumVariable('testabi', 'Test ABI for library function calling', 'aocl',
                 allowed_values=('aocl', 'glibc', 'acml'),
                 map={}, ignorecase=2),  # lowercase always

    EnumVariable('developer', 'A developer friendly mode', 0,
                 allowed_values=('0', '1', '2', '3', '4'),
                 map={'0':0, '1':1, '2':2, '3':3, '4':4}),
)

unknown = vars.UnknownVariables()
if unknown:
    print("Unknown variables:", unknown.keys())
    Exit(1)

env = Environment(variables=vars,
                  CPPDEFINES = {
                      'LIBABI': '${libabi}',
                      'DEVELOPER': '${developer}',
                  }
)

# Add shared top-level headers
env.Prepend(CPPPATH=[Dir('include')])

if GetOption('verbose'):
    def MakeAction(action, string, *args, **kwargs):
        return Action(action, *args, **kwargs)
    print("coming to GetOption")
    env['verbose'] = True
>>>>>>> db70eed... libm: first steps towards using aocc
else:
    MakeAction = Action
    env['verbose'] = False

Export('MakeAction')

#
# Generate configurations and export
#
env['BUILDROOT'] = str(Dir('./build'))
env.SetupConfiguration()
build_root = env['BUILDROOT']

makedirs(build_root, exist_ok=True)

#
# Set up global sticky variables... these are common to an entire build
# tree (not specific to a particular build like ALPHA_SE)
#
global_vars_file = joinpath(build_root, 'variables.global')
global_vars = Variables(global_vars_file, args=ARGUMENTS)
global_vars.AddVariables(
    ('CC', 'C compiler', environ.get('CC', env['CC'])),
    ('CXX', 'C++ compiler', environ.get('CXX', env['CXX'])),
)
# Update env environment with values from ARGUMENTS & global_vars_file
global_vars.Update(env)
help_texts["global_vars"] += global_vars.GenerateHelpText(env)

# Save sticky variable settings back to current variables file
global_vars.Save(global_vars_file, env)

# These objects are not the .obj files or .o files, instead
# class objects or build objectw
objects = []

env.Append(
	INCPATH=['#include']
)
amdlibm = SConscript('src/SConscript',
                       exports = { 'env' : env },
                       duplicate = 0,
                       variant_dir = joinpath(build_root, 'src'))

#
# Build Test lib and associated tests
#
env.Append(
	LIBPATH=['#'+joinpath(build_root,'src')]
)
test_lib_objs = []  			# Will fill at a later date
test_objs = SConscript(dirs='tests',
                       exports = {'env' : env},
                       duplicate = 0,
                       src_dir    = 'tests',
                       variant_dir = joinpath(build_root, 'tests'))

objects += amdlibm + test_objs

# base help text
Help('''
Usage: scons [scons options] [build variables] [target(s)]

Extra scons options:
%(options)s

Global build variables:
%(global_vars)s

%(local_vars)s
''' % localopts.GetHelpTexts())

