class Compiler:
    def __init__(self, prod_mode):
        self.cmd = ''
        self.prod_mode = prod_mode
        self.compile_flags_debug = [
            '-g',
            '-Og',
            '-march=native',
        ]
        self.compile_flags_release = [
            '-Ofast',
            '-march=native',
            '-fipa-pta',
            '-funsafe-loop-optimizations',
            '-flto=4',
        ]
        self.compile_flag_map = {
			'debug': self.compile_flags_debug,
			'release' : self.compile_flags_release
		}

        self.link_flags_debug = []

        self.link_flags_release = self.compile_flags_release

        self.link_flag_map = {
            "debug": self.link_flags_debug,
            "release": self.link_flags_release
        }

        self.cpp_flags_debug = []

        self.cpp_flags_release = []

        self.cpp_flag_map = {
            "debug": self.cpp_flags_debug,
            "release": self.cpp_flags_release
        }
