#!/usr/bin/env python
'''clean up swap files, executables/object code, or other redundant files'''
import os
import sys
from ansicolor import KRED, KGRN, KNRM
to_re_move = ['swn', 'swo', 'swp', 'exe', 'gch', 'o', 'pickle', 'pyc', 'bak']
to_re_move = ["*." + x for x in to_re_move]


def re_move(ext):
    my_files = os.popen('find ./ -name "' + ext + '"').readlines()
    for f in my_files:
        fn = f.strip()
        if os.path.exists(fn):
            a = os.popen("rm "+fn).read()
            print("\tdel " + KRED + fn + KNRM)

for e in to_re_move:
    re_move(e)

re_move('.DS_Store')
