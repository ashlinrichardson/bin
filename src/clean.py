#!/usr/bin/env python
'''clean up swap files, executables/object code, or other redundant files'''
import os
import sys
from ansicolor import KRED, KGRN, KNRM
to_re_move = ['_x3.bin',
              '_x3.hdr',
              '.bin.ml',
              '.a',
              '.swn',
              '.swo',
              '.swp',
              '.exe',
              '.gch',
              '.o',
              '.pickle',
              '.pyc',
              '.bak',
              '.DS_Store']
to_re_move = ["*" + x for x in to_re_move]
to_re_move += ['*_x3.*']

s2_remove = ['S*_L2A_10m_EPSG*',
             'S*_L2A_20m_EPSG*',
             'S*_L2A_60m_EPSG*',
             'S*_L2A_TCI_EPSG*']

to_re_move += s2_remove

def re_move(ext):
    # print('find ./ -name "' + ext + '"')
    my_files = os.popen('find ./ -name "' + ext + '"').readlines()
    for f in my_files:
        fn = f.strip()
        if os.path.exists(fn):
            a = os.popen('rm "' + fn + '"').read()
            print("\tdel " + KRED + fn + KNRM)

for e in to_re_move:
    re_move(e)

re_move('.DS_Store')
