#!/usr/bin/env python2.7
''' ds.py: count number of bytes in present directory'''
import os
import sys
from misc import error, normpath

ds, dd = 0, 0
for root, dirs, files in os.walk(os.getcwd()):
        root = normpath(root)
        # if os.path.isdir(root):

        for f in files:
            # print root + f
            ds += os.stat(root + f).st_size

        for d in dirs:
            print root + d
            dd += os.stat(root + d).st_size

print "files bytes", ds
print "direc bytes", dd
print "total bytes", ds + dd
