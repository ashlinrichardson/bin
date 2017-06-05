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
    print(KMAG + '+' + KRED + 'w ' + KGRN + f.strip() + KNRM)


# check file message
def chkfile(f):
    if not os.path.exists(f):
        error('file not found: ' + f)


# exists
def exists(f):
    return os.path.exists(f)


def chkdir(f):
    return os.path.exists(f) and os.path.isdir(f)


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


# read lines from a file
def readlines(f):
    return open(f).read().strip().split('\n')


# normalized file path (with slash, if directory)
def normpath(p):
    pp = os.path.abspath(p)
    if os.path.isdir(pp):
        pp += '/'
    return pp


# normalized directory name (file member removed).. Given a file, get a standardized parent directory name
def normdir(p):
    return normpath(os.path.dirname(p))


# standardized time stamp string
def timestring():
    import time
    t = time.localtime()
    return (('%04d' % t.tm_year) +
            ('%02d' % t.tm_mon) +
            ('%02d' % t.tm_mday) +
            ('%02d' % t.tm_hour) +
            ('%02d' % t.tm_min) +
            ('%02d' % t.tm_sec))


def require_gdal():
    if os.popen("which gdalinfo").read().strip() == "":
        error('could not find gdal:: gdalinfo')

def config_file(nrow, ncol):
    return "Nrow\n"+str(nrow)+"\n"+"---------\nNcol\n"+str(ncol)+"\n---------\nPolarCase\nbistatic\n---------\nPolarType\nfull"

# test
if __name__ == '__main__':
    error("this is an error message")
