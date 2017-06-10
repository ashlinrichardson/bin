#!/usr/bin/env python
import os
import sys
from fl0w3r import error


def to_matlab(a):
    s = "["
    for i in range(0, len(a) - 1):
        s += str(a[i]) + ","
    s += str(a[len(a) - 1])
    s += "]"
    return s

args = sys.argv
if len(args) < 3:
    error("asdcv.py: transpose a data table (cols) [infile] [outfile]")

lines = open(args[1]).readlines()
o = open(args[2], "wb")
a, b, c = [], [], []

for i in range(1, len(lines)):
    f = ((lines[i].strip()).strip(','))
    if(len(f.split(',')) > 1):
        f = f.split(',')
    elif(len(f.split('\t')) > 1):
        f = f.split('\t')
    else:
        f = f.split()

    a.append(f[0])
    b.append(f[1])
    if(len(f) > 2):
        c.append(f[2])

print "len(a):", len(a), "len(b):", len(b), "len(c):", len(c)

o.write(str(len(a)) + " " + str(len(b)) + " " + str(len(c)) + "\n")
for i in range(0, len(a)):
    o.write(str(a[i]) + " ")
o.write("\n")

for i in range(0, len(b)):
    o.write(str(b[i]) + " ")
o.write("\n")

for i in range(0, len(c)):
    o.write(str(c[i]) + " ")
