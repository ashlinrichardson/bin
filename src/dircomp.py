#!/usr/bin/env python27
'''20180606: compare two directories... highlight (in two colors),
files/folders in the first directory,
1) not appearing in the second
2) appearing in the second, but diferring in contents'''
import os
import sys
import filecmp
from fl0w3r import normpath, wopen, error, run, chkfile, args, exists, ext
from ansicolor import KYEL, KMAG, KRED, KGRN, KNRM

if len(args) < 3:
    error('dircomp.py: compare two directory structures: report any files/' +
          'folders in dir1, not in dir2 (note: need to run this both ways)' +
          '\n\tusage: dircomp [directory 1] [directory 2]')

dir1 = normpath(args[1])
dir2 = normpath(args[2])

# print dir1
# print dir2

print KMAG + "files/folders in " + KYEL + dir1 + KMAG + " but not in " +\
      KYEL + dir2 + KNRM + " in " + KRED + "red, " + KNRM
print "files in dir1 also in dir2 that don't match, in " + KYEL +\
      "yellow" + KNRM


def compare(dir1, dir2):
    basedir_l = len(dir1) - 1  # don't count slash..

    for root, dirs, files in os.walk(dir1):
        root = normpath(root)

        other_root = None

        if os.path.isdir(root):
            other_root = root[basedir_l:]
            if other_root[0] != "/":
                error('other_root[0] != "/": \n\t' + other_root)
            if dir2[0] != "/":
                error('dir2[0] != "/"')
            other_root = dir2 + other_root[1:]
            if not exists(other_root):
                print KRED + root + KNRM
        else:
            error('not(isdir(root))')

        for f in files:
            other_f = other_root + f
            if not exists(other_f):
                print KRED + root + f + KNRM
            else:
                if not filecmp.cmp(root + f, other_f):
                    print KYEL + other_f + KNRM


compare(dir1, dir2)
