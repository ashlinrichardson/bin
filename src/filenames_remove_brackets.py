#!/usr/bin/env python3
''' remove brackets from filenames in current directory tree'''
import os
import sys

def err(m):
    print("Error: " + m)
    sys.exit(1)

sep = os.path.sep
normpath = os.path.normpath

d = os.getcwd()

for root, dirs, files in os.walk(d):
    root = normpath(root)
    if len(root.split(' ')) > 1:
        err("space detected in path:\n" + root)

for root, dirs, files in os.walk(d):
    root = normpath(root)
    if root[-1] != sep:
        root += sep

    for f in files:
        ff = root + f

        for c in ['[', ']', '{', '}', '(', ')']:
            if len(f.split(c)) > 1:
                f = f.replace(c, '')
        
        fff = root + f
        print('os.rename("' + ff + '",\n          "' + fff + '")')
        os.rename(ff, fff)
