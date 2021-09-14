#!/usr/bin/env python3
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

        if len(f.split(' ')) > 1:
            fff = root + f.replace(' ', '_')
            print('os.rename("' + ff + '",\n          "' + fff + '")')
            os.rename(ff, fff)



