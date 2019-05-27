# Copyright (C) Prem Mallappa
#
# Author: Prem Mallappa <prem.mallappa@gmail.com>
#
from os.path import join as joinpath
from os import environ

from SCons.Node.FS import Dir
from SCons.Variables import Variables as SVariables
from SCons.Script import Environment, ARGUMENTS, BoolVariable, EnumVariable, GetOption
from SCons.Util import AddMethod

from . import cfg

class DefaultCfg(object):
    def __default_store(self, option, opt_str, value, parser):
        #print("called inside default store")
        setattr(parser.values, option.dest, value)
        print(option, opt_str, value)
        self.dummyenv[option.dest] = value

    def __init__(self, build_root=None):
        if build_root == None:
            self.projpath = Dir('#build')
        else:
            self.projpath = str(build_root)

        print('projpath', build_root)
        self.__help_texts = {
            "global_vars": "",
            "options"    : '',
            'local_vars' : ''
        }
        self.dummyenv = {}

        self.def_env_file = "global.vars"
        self.defvars = None
        self.opts = None

        self.AddOptions()
        self.AddVariables()

        self.defenv = Environment(variables = self.defvars)

        self.Check()

    def AddOptions(self):
        opts = cfg.LocalOption()

        opts.Add('--verbose', dest='verbose', nargs=1, action='callback',
                 callback=self.__default_store,
                 default='none', type='int',
                 help='Print full tool command lines')
        opts.Add('--debug_mode',
                 callback=self.__default_store,
                 dest='debug_mode', nargs=1, action='callback',
                 default='none', type='str',
                 #choices=['none', 'libs', 'tests', 'all'],
                 help='Enable Debug mode [none, libs, tests, all] [default:none]')
        opts.Add('--libabi', dest='libabi', nargs=1, action='callback',
                 callback=self.__default_store,
                 default='aocl', type='choice',
                 choices=['aocl', 'glibc', 'libm', 'svml'],
                 help="""Compile tests to call this abi
                      aocl  - AOCL, functions prefixed with 'amd_*'
                      glibc - GLIBC abi calls, functions prefixed with '__ieee_*'
                      libm  - Usual C Standard library calls: exp, pow, sin, cos etc.
                      svml  - Intel SVML calls
                      """)
        opts.Add('--developer', dest='developer', nargs=1, action='callback',
                 callback=self.__default_store,
                 type='choice',
                 choices=['1', '2', '3', '4'],
                 help='Enable Developer mode [1, 2, 3, 4]')
        opts.Add('--build', dest='build', nargs=1, action='callback',
                 callback=self.__default_store,
                 type='choice',
                 choices=['release', 'developer', 'debug'],
                 help='Enable build type [release, developer, debug]')

        self.opts = opts

    def AddVariables(self):
        defvars = SVariables(self.def_env_file, args=ARGUMENTS)
        defvars.AddVariables(
            BoolVariable('verbose',  'Increase verbosity', 0),
            EnumVariable('debug_mode',    'debug output and symbols', 'no',
                         allowed_values=('no','libs', 'tests', 'all'),
                         map={}, ignorecase=0),  # case sensitive
	        # test abi makes tests to call out for given library call
            EnumVariable('libabi', 'Test ABI for library function calling', 'aocl',
                         allowed_values=('aocl', 'glibc', 'libm', 'acml'),
                         map={}, ignorecase=2),  # lowercase always
            EnumVariable('developer', 'A developer friendly mode', 0,
                         allowed_values=('0', '1', '2', '3', '4'),
                         map={'0':0, '1':1, '2':2, '3':3, '4':4}),
            EnumVariable('build', 'Build type shortcut', 'release',
                        allowed_values=('release', 'developer', 'debug'), ignorecase=2)
        )

        self.defvars = defvars

    def Check(self):
        #import pdb
        #for key in self.dummyenv:
        self.defvars.Update(self.defenv)
        #pdb.set_trace()
        self.defvars.Update(self.defenv, args=self.dummyenv)
	
        unknown = self.defvars.UnknownVariables()
        if unknown:
            print("Unknown variables:", unknown.keys())
            #Exit(1)

    def GetDefaultEnv(self):
        from os import makedirs
        #print('calling Environment')
        env = self.defenv
        helper.SetupConfiguration(env)
        makedirs(env['BUILDROOT'], exist_ok=True)
        self.def_env_file = joinpath(env['BUILDROOT'], self.def_env_file)

        self.__help_texts["options"] = self.opts.GetHelpTexts()
        self.__help_texts["local_vars"] = self.defvars.GenerateHelpText(env)

        env.Append(
            CPPDEFINES = { 'LIBABI': env['libabi']})

        #print("developer=> ", env['developer'])

        if env['libabi'] == 'glibc':
            env.Append(CPPDEFINES = {'ENABLE_GLIBC_API' : 1})
        elif env['libabi'] == 'libm':
            env.Append(CPPDEFINES = {'ENABLE_LIBM_API' : 1})
        elif env['libabi'] == 'acml' or env['libabi'] == 'aocl':
            env.Append(CPPDEFINES = {'ENABLE_AMDLIBM_API' : 1})

        if env['developer'] != 0:
            env.Append(
                CPPDEFINES = {'DEVELOPER' : env['developer']})

        self.defvars.Save(self.def_env_file, env)
        return env

    def GetHelpTexts(self):
        return self.__help_texts
