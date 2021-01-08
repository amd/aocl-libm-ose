#
# Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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

from SCons.Variables import Variables as SVariables
from SCons.Script import AddOption

class LocalOption:
    def __init__(self):
        self.__help_texts = ""
        pass

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
        self.__help_texts += help + "\n"

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

