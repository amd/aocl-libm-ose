import os
from scripts.cfg import projpath

def strip_build_path(path, env):
    path = str(path)
    build_base = projpath.BUILDDIR
    variant_base = env['BUILDROOT'] + os.path.sep
    if path.startswith(variant_base):
        path = path[len(variant_base):]
    elif path.startswith(build_base):
        path = path[len(build_base):]
    return path

class Transform():
    def __init__(self, tool):
        self.format = (" [%8s] " % tool) \
            +   "%s" \
            +   "%s" \
            + " -> " \
            +   "%s"

        self.max_sources = 99

    def __call__(self, target, source, env, for_signature=None):
        # truncate source list according to max_sources param
        source = source[0:self.max_sources]
        def strip(f):
            return strip_build_path(str(f), env)
        if len(source) > 0:
            srcs = map(strip, source)
        else:
            srcs = ['']

        tgts = map(strip, target)
        # surprisingly, os.path.commonprefix is a dumb char-by-char string
        # operation that has nothing to do with paths.
        com_pfx = os.path.commonprefix(srcs + tgts)
        com_pfx_len = len(com_pfx)

        def fmt(files):
            f = map(lambda s: s[com_pfx_len:], files)
            return ', '.join(f)

        return self.format % (com_pfx, fmt(srcs), fmt(tgts))

__all__ = ['Transform']

