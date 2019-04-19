from SCons.Variables import Variables as SVariables
from SCons.Script import AddOption,Import

class LocalOption:
    def __init__(self, htexts):
        self.__help_texts = htexts

    def Add(self, *args, **kwargs):
        col_width = 30

        help = "  " + ", ".join(args)
        if "help" in kwargs:
            length = len(help)
            if length >= col_width:
                help += "\n" + " " * col_width
            else:
                help += " " * (col_width - length)
            help += kwargs["help"]
        self.__help_texts["options"] += help + "\n"

        AddOption(*args, **kwargs)

    def GetHelpTexts(self):
        return self.__help_texts


class Variables(SVariables):
    def __init__(self, files=[], args={}, is_global=1):
        self.required = []
        super(self.__class__,self).__init__(files, args, is_global)

    def Add(self, key, help="", default=None, validator=None, converter=None, required=False):
        SVariables.Add(self, key, help, default, validator, converter)
        if required:
            print("adding required option ", key[0])
            self.required.append(key[0])

    def Update(self, env):
        print("required options are: ", self.required)
        SVariables.Update(self, env)
        for requirement in self.required:
            if not env.has_key(requirement):
                print('violation: ', requirement)


