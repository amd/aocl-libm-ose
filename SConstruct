# Copyright (C) 2018, AMD. All rights Reserved
#
# Author: Prem Mallappa <pmallapp@amd.com>

import os
from os.path import join

from scripts.cfg import config
from scripts.cfg import projpath
from scripts.utils import Transform

cfg		= config.current_config()
builddir	= '#build'
projroot	= projpath.PROJROOT

Help("""
	Type:
	        scons .         build and test everything
	        scons test      build the software
	        scons src       run the tests
	        scons web       build the web pages
	""")

print("BULDING in: ", builddir)

env = Environment(
		ENV       = {'PATH': os.environ['PATH']},
		CPPPATH   = ['#include', '#include/libm'],
		LIBPATH   = [join(builddir, 'src')]
		)

if ARGUMENTS.get('verbose') != "1":
	env['CCCOMSTR']     = "[CC] => $TARGET"
	env['ASCOMSTR']     = "[AS] => $TARGET"
	env['ARCOMSTR']     = "[AR] => $TARGET"
	env['ASPPCOMSTR']   = "[AS] => $TARGET"
	env['LINKCOMSTR']   = "[LINK] => $TARGET"
	env['RANLIBCOMSTR'] = "[RANLIB] => $TARGET"
	env["SHCCCOMSTR"]   = "[SHCC] => $SOURCE"
	env["SHLINKCOMSTR"] = "[SHLINK] => $TARGET"

if ARGUMENTS.get('debug', 0):
	env.Append(CCFLAGS = ' -g')

# These objects are not the .obj files or .o files, instead
# class objects or build objectw
objects = []

lib_objs = SConscript('src/SConscript',
			exports = { 'env' : env },
			duplicate = 0,
			variant_dir = join(builddir, 'src'))

test_lib_objs = []  			# Will fill at a later date

test_objs = SConscript( dirs='tests',
			exports = { 'env' : env },
			duplicate = 0,
			src_dir    = 'tests',
			variant_dir = join(builddir, 'tests'))

objects += lib_objs + test_objs

if COMMAND_LINE_TARGETS:
   targets = COMMAND_LINE_TARGETS
else:
   targets = DEFAULT_TARGETS

#Depends(test_objs, lib_objs)
#Depends(objects, targets)

