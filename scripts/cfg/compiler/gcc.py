from . import Compiler
class Gcc(Compiler):
    def __init__(self, prod_mode):
        super(Gcc, self).__init__(prod_mode)
        self.cmd = 'gcc'	
        self.compile_flags_debug = [
            '-g',
            '-Og',
            '-march=native',
        ]
        self.compile_flags_release = []
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
        self.cpp_flag_map = {
            "debug":   self.cpp_flags_debug,
            "release": self.cpp_flags_release
        }
	
        self.cpp_flags_debug = []
        self.cpp_flags_release = []
		
        self.warnings = [	
	        '-Wall',		
	        '-Wextra',		
	        '-Wpedantic',		
#	'-Wabi',		
	        '-Wcast-align',		
	        '-Wcast-qual',		
	        '-Wconversion',		
	        '-Wctor-dtor-privacy',		
	        '-Wdisabled-optimization',		
	        '-Wdouble-promotion',		
#	'-Weffc++',		
#	'-Wfloat-equal',		
	        '-Wformat=2',		
	        '-Winit-self',		
	        '-Winvalid-pch',		
#	'-Wlogical-op',		
	        '-Wmissing-declarations',		
	        '-Wmissing-include-dirs',		
	        '-Wnoexcept',		
	        '-Wodr',		
	        '-Wold-style-cast',		
	        '-Woverloaded-virtual',		
#	'-Wpadded',		
	        '-Wredundant-decls',		
	        '-Wshadow',		
	        '-Wsign-conversion',		
#	'-Wsign-promo',		
#	'-Wsuggest-final-methods',		
#	'-Wsuggest-final-types',		
	        '-Wstrict-null-sentinel',		
#	'-Wstrict-overflow=5',		
	        '-Wswitch-default',		
            # -Wswitch-enum needs every switch statement to be handled explicitly.		
            #       It would be useful if the language had some mechanism		
            #       to activate this on specified switch statements (to ensure that future		
            #       changes to the enum are handled everywhere that they need to be), but it's		
            #       overkill for an "all-or-nothing" setting.		
#	'-Wswitch-enum',		
	        '-Wtrampolines',		
	        '-Wundef',		
            # -Wunsafe-loop-optimizations causes too many spurious warnings. It may be		
            #       useful to apply this one periodically and manually verify the results.		
            #       It is also issued warning for the constructor of a const array of const		
            #       std::string (where there is no loop in user code).		
            #		
#	'-Wunsafe-loop-optimizations',		
            # -Wuseless-cast is incompatible with BOUNDED_INTEGER_CONDITIONAL		
#	'-Wuseless-cast',		
	        '-Wvector-operation-performance',		
            # -Wzero-as-null-pointer-constant does not work with the operator<=> emulation		
#	'-Wzero-as-null-pointer-constant',		
	        '-Werror',
            '-Wlto-type-mismatch',		
        ]


    def Cmd(self):
        return self.cmd
    def CFlags(self):
        return self.compile_flag_map[self.prod_mode]

class ICC(Compiler):
    def __init__(self):
        pass
