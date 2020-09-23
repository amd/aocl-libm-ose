from . import Compiler

class LLVM(Compiler):
    def __init__(self, prod_mode):
        super(LLVM, self).__init__(prod_mode)
        self.cmd = 'clang'
        self.cxxcmd = 'clang++'
        self.compile_flags_debug = [
            '-g',
            '-Og',
            '-march=native',
        ]
        self.compile_flags_release = [
            '-ffp-contract=fast', # Needed to generate FMA instructions for vector routines
            #'-Ofast',
            # '-march=native',
            # '-fipa-pta',
            # '-funsafe-loop-optimizations',
            # '-flto=4',
        ]
        self.compile_flag_map = {
            'debug': self.compile_flags_debug,
            'release' : self.compile_flags_release
        }

        self.link_flags_debug = ['-fuse-ld=ld']
        self.link_flags_release = ['-fuse-ld=ld']

        self.link_flag_map = {
            "debug": self.link_flags_debug,
            "release": self.link_flags_release
        }

        self.cpp_flag_map = {
            "debug":   self.cpp_flags_debug,
            "release": self.cpp_flags_release
        }

        self.cpp_flags_debug = []
        self.cpp_flags_release = []

    def Cmd(self):
        return self.cmd

    def CxxCmd(self):
        return self.cxxcmd

    def CFlags(self):
        return self.compile_flag_map[self.prod_mode]

    def LDFlags(self):
        return self.link_flag_map[self.prod_mode]
