#!/usr/bin/env python
import os
import sys
from misc import run, error, exists, args

if len(args) < 2:
    error('usage: src2pdf [source code file name]')

fn = args[1]
fnbase = (os.path.splitext(os.path.abspath(fn)))[0]

run('vim ' + fnbase + '.ps -c ":hardcopy > '   '" -c ":q"')
run('ps2pdf ' + fnbase + '.ps ' + fnbase + '.pdf')
