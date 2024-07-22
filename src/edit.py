#!/usr/bin/env python
'''workflow: open a file in vim: if not a header file, "compile".
by ashlin richardson 20170621.
20170622 added searching for files, based on language-level
priority. should add auto-commit'''
import os
import sys
from copy import deepcopy
from misc import run, error, normpath, args, exists

pwd = normpath(os.getcwd())
home = normpath(os.path.expanduser('~/bin/src/'))
if pwd != home:
    error('edit.py to be run from home')

if len(args) < 2:
    error('edit.py: Usage: edit.py [name of file to edit]')

fn, compil = normpath(args[1]), ['py', 'c', 'cpp']
suppor = deepcopy(compil)
suppor.append('h')

ext = fn.split('.')[-1].strip()
base = fn.split('/')[-1].strip()

if not exists(fn):
    # embellish filename (if already exists)
    cmd = 'find ./ -name "' + base + '.*"'
    friends = os.popen(cmd).readlines()
    if len(friends) == 1:
        fn = normpath(friends[0]).strip()
        base = fn.split('/')[-1].strip()
        ext = base.split('.')[-1].strip()
    else:
        base = {f.split('/')[-1].strip(): f for f in friends}
        exts = {b.split('.')[-1].strip(): base[b] for b in base.keys()}

        # should compose dicts instead:
        if 'py' in exts:
            fn, ext = exts['py'], 'py'
        elif 'cpp' in exts:
            fn, ext = exts['cpp'], 'cpp'
        elif 'c' in exts:
            fn, ext = exts['c'], 'c'
        elif 'h' in exts:
            fn, ext = exts['h'], 'h'
        else:
            error('supported extensions: ' + (' '.join(suppor)))

if not(ext in suppor):
    error('extension not supported: ' + str(ext))

run('vim ' + fn)

if ext in compil:
    run('python compile.py ' + fn)

if ext == 'py':
    run('pep8 ' + fn)
