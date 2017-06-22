#!/usr/bin/env python
'''workflow: open a file in vim: if not a header file, "compile".
by ashlin richardson 20170621'''
import os
import sys
from copy import deepcopy
from fl0w3r import run, error, normpath, args

pwd = normpath(os.getcwd())
home = normpath(os.path.expanduser('~/GitHub/fl0w3r/src/'))
if pwd != home:
    error('edit.py to be run from home')

if len(args) < 2:
    error('edit.py: Usage: edit.py [name of file to edit]')

fn, compil = normpath(args[1]), ['py', 'c', 'cpp']
suppor = deepcopy(compil)
suppor.append('h')
ext = fn.split('.')[-1]

if not(ext in suppor):
    print 'edit.py: error: file not supported: ' + fn
    error('extension not supported: ' + str(ext))

run('vim ' + fn)

if ext in compil:
    run('python compile.py ' + fn)
