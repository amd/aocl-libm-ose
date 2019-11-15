#
# Copyright (C) 2019 Prem Mallappa.
#
#
# Author: Prem Mallappa<prem.mallappa@gmail.com>
#         Thanks to original authors at Gem5, most of the code belongs to them.
#
#
from SCons.Script import GetOption

import os

def strip_build_path(path, env):
    path = str(path)
    build_base = 'build/'
    variant_base = env['BUILDROOT'] + os.path.sep
    #print('buildroot', env['BUILDROOT'])
    if path.startswith(variant_base):
        path = path[len(variant_base):]
    elif path.startswith(build_base):
        path = path[len(build_base):]
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
        tgts = map(strip, target)
        newsrcs = list(srcs)
        newtgts = list(tgts)

        from os.path import commonprefix
        # surprisingly, os.path.commonprefix is a dumb char-by-char string
        # operation that has nothing to do with paths.
        com_pfx = commonprefix(newsrcs + newtgts)

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
        UpdateEnvComStr(env)

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

