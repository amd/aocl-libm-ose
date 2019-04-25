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
from scripts.cfg import DefaultCfg

defcfg = DefaultCfg(build_root=Dir('#build', create=True))

env = defcfg.GetDefaultEnv()

# Add shared top-level headers
env.Prepend(CPPPATH=[Dir('include')])

if GetOption('verbose'):
    def MakeAction(action, string, *args, **kwargs):
        return Action(action, *args, **kwargs)
    print("coming to GetOption")
    env['verbose'] = True
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

gvars = Variables(defcfg.def_env_file, args=ARGUMENTS)
gvars.AddVariables(
    ('CC', 'C compiler', environ.get('CC', env['CC'])),
    ('CXX', 'C++ compiler', environ.get('CXX', env['CXX'])),
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
	INCPATH=['#include']
)
amdlibm = SConscript('src/SConscript',
                       exports = { 'env' : libenv },
                       duplicate = 0,
                       variant_dir = joinpath(build_root, 'src'))

targets += amdlibm

#
# Build Test lib and associated tests
#
testenv = env.Clone()
testenv.Append(
	LIBPATH=['#'+joinpath(build_root,'src')]
)
test_lib_objs = []  			# Will fill at a later date
test_objs = SConscript(dirs='tests',
                       exports = {'env' : testenv},
                       duplicate = 0,
                       src_dir    = 'tests',
                       variant_dir = joinpath(build_root, 'tests'))


if 'tests' in COMMAND_LINE_TARGETS:
    targets += test_objs

Progress('\r', overwrite=True)
Default(targets)

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

