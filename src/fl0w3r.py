#!/usr/bin/env python2.7
'''
'''
import os
import sys
from ansicolor import KRED, KGRN, KNRM, KMAG


# error message and quit
def error(msg):
    print KRED + "Error: " + KGRN + msg.strip() + KNRM
    sys.exit(1)


# printing message
def printw(f):
    print(KMAG + '+' + KRED + 'w' + KGRN + f.strip() + KNRM)


# check file message
def chkfile(f):
    if not os.path.exists(f):
        error('file not found: ' + f)


# exists
def exists(f):
    return os.path.exists(f)


# open write mode
def wopen(fn):
    f = open(fn, 'w')
    if(f is None):
        error('no write access to file: ' + fn)
    return f


# read lines from a file
def readlines(fn):
    chkfile(fn)
    return open(fn).readlines()


# run a system command
def run(cmd):
    print KGRN + str(cmd).strip() + KNRM
    a = os.system(cmd)
    if(a != 0):
        error('command failed')


# test
if __name__ == '__main__':
    error("this is an error message")
