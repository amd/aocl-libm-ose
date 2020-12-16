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

from SCons.Script import GetOption

import os

def strip_build_path(path, env):
    path = str(path)
    #print('path before-> ' , path)
    bld_root = env['BUILDROOT'] + '/'
    #bld_root_src = bld_root + 'src' + '/'
    bases = ['src/', 'tests/', bld_root] #, bld_root_src]
    for b in bases:
        if path.startswith(b):
            path = path[len(b):]
            #break
    #print('path', path)
    return path

#
# Parts are taken from Gem5, many thanks to Gem5 authors
class Transform(object):
    def __init__(self, tool, hidesrc = False, max_sources=99):
        self.hidesrc =  hidesrc
        if hidesrc:
            self.format = (" [%6s] " % tool) + "%s"
        else:
            self.format = (" [%6s] " % tool) + "%s" " => " + "%s"
        self.max_sources = max_sources

    def __call__(self, target, source, env, for_signature=None):
        #print('call called', target, source)
        source = source[0:self.max_sources]
        def strip(f):
            return strip_build_path(str(f), env)

        if len(source) > 0:
            srcs = map(strip, source)
        else:
            srcs = ['']


        newsrcs = list(srcs)
        #import pdb
        #pdb.set_trace()
        tgts = map(strip, target)
        #print("==>", srcs, tgts)
        newtgts = list(tgts)

        from os.path import commonprefix
        # surprisingly, os.path.commonprefix is a dumb char-by-char string
        # operation that has nothing to do with paths.
        com_pfx = '' # commonprefix(newsrcs + newtgts)
        com_pfx_len = len(com_pfx)

        if com_pfx:
            # do some cleanup and sanity checking on common prefix
            if com_pfx[-1] == ".":
                # prefix matches all but file extension: ok
                # back up one to change 'foo.cc -> o' to 'foo.cc -> .o'
                com_pfx = os.path.dirname(com_pfx[0:-1]) + os.path.sep
            elif com_pfx[-1] == "/":
                # common prefix is directory path: OK
                pass
            else:
                src0_len = len(newsrcs[0])
                tgt0_len = len(newtgts[0])

                if src0_len == com_pfx_len:
                    # source is a substring of target, OK
                    pass
                elif tgt0_len == com_pfx_len:
                    # target is a substring of source, need to back up to
                    # avoid empty string on RHS of arrow
                    sep_idx = com_pfx.rfind(".")
                    if sep_idx != -1:
                        com_pfx = com_pfx[0:sep_idx]
                    else:
                        com_pfx = ''
                elif src0_len > com_pfx_len and newsrcs[0][com_pfx_len] == ".":
                    # still splitting at file extension: ok
                    pass
                else:
                    # probably a fluke; ignore it
                    com_pfx += os.path.sep

        com_pfx_len = len(com_pfx)

        def fmt(files):
            #f = []
            #for s in files:
            #    if s.startswith(com_pfx):
            #        f.append(s[com_pfx_len:])
            #    else:
            #        f.append(s)

            f = map(lambda s: s[com_pfx_len:] if s.startswith(com_pfx) else s, files)
            return ', '.join(f)

        if self.hidesrc:
            return self.format%(fmt(newtgts))

        return self.format%(fmt(newsrcs), fmt(newtgts))

    def __str__(self):
        return self.format

def UpdateEnvComStr(env):
    if not env['verbose']:
        env['CCCOMSTR']     = Transform('CC')
        env['CXXCOMSTR']    = Transform('CXX')
        env['ASCOMSTR']     = Transform('AS')
        env['ARCOMSTR']     = Transform('AR', hidesrc=True)
        env['ASPPCOMSTR']   = Transform('AS')
        env['LINKCOMSTR']   = Transform('LINK', hidesrc=True)
        env['RANLIBCOMSTR'] = Transform('RANLIB', hidesrc=True)
        env["SHCCCOMSTR"]   = Transform('SHCC')
        env["SHLINKCOMSTR"] = Transform('SHLINK', hidesrc=True)


def MakeInstallRoot(env):
    """Build root has
       build/<libabi>-<debug/release/developer>
    """
    try:
        inst = env['prefix']
    except KeyError:
        inst = '#install'

    env['INSTALL_PREFIX'] = inst

def MakeBuildRoot(env):
    """Build root has
       build/<libabi>-<debug/release/developer>
    """
    try:
        b = env['BUILDROOT']
    except KeyError:
        b = 'build'

    build_mode_to_dir = {
        'debug'     : '-debug',
        'release'   : '-release',
        'developer' : '-dev'
    }

    b += '/' + '%s'%(env['libabi'])
    if  env['debug_mode'] != GetOption('debug_mode'):
        dbg = GetOption('debug_mode')
    else:
        dbg = env['debug_mode']

    if dbg == 'libs' or dbg == 'all':
        b += "%s"%build_mode_to_dir['debug']
    else:
        if env['developer'] > 0:
            b += "%s%d"%(build_mode_to_dir['developer'],env['developer'])
        else:
            b += build_mode_to_dir['release']

    env['BUILDROOT'] = b
    #print('buildroot', b)

def SetupConfiguration(env):
        """Build the test program;
        prepends "test_" to src and target,
        and puts target into testdir."""
        #print(env.Dump())
        MakeBuildRoot(env)
        MakeInstallRoot(env)
        UpdateEnvComStr(env)
        env['compiler'] = GetOption('compiler')

        if env['debug_mode'] != 'no':
            env.Append(CPPDEFINES = {'DEBUG': '1'})

def PrintBanner(env):
        print("====================== Configuration ======================")
        print("""
        Compiler     : %-20s  Build    : %s
        ABI          : %-20s  Developer: %s
        ToolchainDir : %s
        BuildDir     : %s
        """% (
            env['compiler'], env['build'],
            env['libabi'], env['developer'],
            env['toolchain_base'],
            env['BUILDROOT']
        ))
        print("===========================================================")

