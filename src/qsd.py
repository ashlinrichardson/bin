#!/usr/bin/python
'''unknown date revised 20170826'''
import os
import sys
from string import *
from os.path import *

if len(sys.argv) != 2:
    print "Usage: eh [input directory]"
    sys.exit(1)

cwd = sys.argv[1]  # MAIN_DIRECTORY + "/matlab_out/"
cwd = abspath(realpath(normcase(normpath(cwd))))

if cwd[len(cwd) - 1] != '/':
    cwd = cwd + '/'

config_file = cwd + "config.txt"
print "Attempting to open config.txt file:", config_file

g = open(config_file)
gl = g.readlines()
g.close()
nlin = int(gl[1].strip())
ncol = int(gl[4].strip())

# this is bad code and needs to be improved
os.system("ls " + cwd + "*.bin > ls12341234.txt")
f = open("ls12341234.txt")
bins = f.read()
f.close()

bins = bins.replace("\n", " ")
bins = bins.strip().split(" ")
a = bins[0].split(".")

if a[len(a) - 1] == "bin":
    pass
else:
    print "no bin files in directory"
    sys.exit(1)

os.system("rm -f ls12341234.txt")
for b in bins:
    print b
    os.system("qs " + str(b) + " " + str(cwd))

sys.exit(0)
