# Copyright (C) Prem Mallappa
#
# Author: Prem Mallappa <prem.mallappa@gmail.com>
#
from os.path import join as joinpath
from os import environ

from SCons.Node.FS import Dir
from SCons.Variables import Variables as SVariables
from SCons.Script import Environment, ARGUMENTS, BoolVariable, EnumVariable, PathVariable,GetOption
from SCons.Util import AddMethod

from . import cfg
from .compiler import gcc, llvm, icc
#from . import compiler

class AlmCfg(object):
    def __default_store(self, option, opt_str, value, parser):
        #print("called inside default store")
        setattr(parser.values, option.dest, value)
        #print(option, opt_str, value)
        self.dummyenv[option.dest] = value

    def __init__(self, build_root=None):
        if build_root == None:
            self.projpath = GetOption('build-root')
            self.projpath = Dir('#build')
        else:
            self.projpath = str(build_root)

        #print('projpath', build_root)
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

        self.defenv = Environment(variables = self.defvars,
                                  ENV = {'PATH' : environ['PATH']})

        #self.Check()
        for key in self.defvars.keys():
            print(key)


        self.opts = AlmOptions()
        self.vars = AlmVariables()

    def Check(self):
        #import pdb
        #for key in self.dummyenv:
        self.defvars.Update(self.defenv)
        #pdb.set_trace()
        self.defvars.Update(self.defenv, args=self.dummyenv)

        unknown = self.defvars.UnknownVariables()
        if unknown:
            print("ALM: build: Unknown variables:", unknown.keys())
            #Exit(1)

        #if debug_mode is mentioned assume build type debug
        if self.defenv['debug_mode'] != 'no':
            self.defenv['build'] = 'debug'

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

        cmpiler = compiler.gcc.Gcc(self.defenv['build'],
                                  bvars=self.defvars,
                                  opts=self.opts)
        #print(env['compiler'])
        if env['compiler'] == 'aocc' or env['compiler'] == 'llvm':
            cmpiler = compiler.llvm.LLVM(self.defenv['build'])

        env.Replace(
            CC = cmpiler.Cmd(),
            CCFLAGS = cmpiler.CFlags(),
            LINKFLAGS = cmpiler.LDFlags(),
        )

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
        self.Check()
        return env

    def GetHelpTexts(self):
        return self.__help_texts
