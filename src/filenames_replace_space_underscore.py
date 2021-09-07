#!/usr/bin/env python3
import os
import sys
sep = os.path.sep
normpath = os.path.normpath

d = os.getcwd()
for root, dirs, files in os.walk(d):
    root = normpath(root)
    if root[-1] != sep:
        root += sep

    for f in files:
        print(root + f)



