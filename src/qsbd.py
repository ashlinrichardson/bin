#!/usr/bin/python
'''run qsb (quick stats box) on all the bin files in a directory
unknown date; revised 20170826'''
import os
import sys
import time
import array
import string
from math import sqrt
from string import *
from os.path import *

if len(sys.argv) != 3:
    print ("Usage: qsbd [box file]" +
           " [path to dir. containing config.txt, and binary files to read]")
    print ("\tNote: binary files and config.txt file must be" +
           "present in the same directory, and binary files must end in .bin")
    sys.exit(1)

boxfile = sys.argv[1]
cwd = sys.argv[2]
boxfile = abspath(boxfile)
cwd = abspath(realpath(normcase(normpath(cwd))))
if cwd[len(cwd) - 1] != '/':
    cwd = cwd + '/'

retcode = os.system("ls -1 " + cwd + "*.bin > " + cwd + ".ls1234.txt")
f = open(cwd + ".ls1234.txt")
lines = f.readlines()
f.close()
retcode = os.system("rm -rf " + cwd + ".ls1234.txt")

print ("xstart ystart xwidth ywidth min max " +
       " mean stdev #pixels filename boxnumber")
cmds = []
for l in lines:
    c = "qsb " + l.strip() + " " + boxfile + " " + cwd
    cmds.append(c)
# print "xstart ystart xwidth ywidth min max
#   mean stdev #pixels filename boxnumber"
for c in cmds:
    retcode = os.system(c)
