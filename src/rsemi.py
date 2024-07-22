#!/usr/bin/env/python
'''rsemi.py: remove right semicolons from e.g., python files
by ashlin richardson 20170822'''
from misc import args, error, exists, wopen

if len(args) < 2:
    error('rsemi.py: usage: rsemi [input file.py]')

fn = args[1]
if not exists(fn):
    error('rsemi.py: file not found: ' + str(fn.strip()))
if fn[-2:] != 'py':
    error('rsemi.py: intended for python code')

tf = fn + '.bak'
dat = open(fn).read()
wopen(tf).write(dat)
lines = dat.split('\n')

for i in range(0, len(lines)):
    lines[i] = lines[i].rstrip(';')

wopen(fn).write('\n'.join(lines))
