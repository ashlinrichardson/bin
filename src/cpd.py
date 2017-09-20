#!/usr/bin/env python
'''20170907 by a. richardson'''
import os
import sys

v = "/Volumes/"
args = sys.argv
if(len(args) < 2):
    print "cpd.py: [filename in present path]"
    sys.exit(1)
pwd = os.path.abspath(os.path.normpath(os.getcwd())) + '/'
print "pwd", pwd

fn = args[1]
if not os.path.exists(fn):
    print "can't find file: " + str(fn)
    sys.exit(1)

fn = os.path.abspath(os.path.normpath(fn))
print "fn", fn

if(fn[0:17] != v + "envwwwd/"):
    print "error: ", str([fn[0:17]])
    sys.exit(1)

if(pwd[0:17] != v + "envwwwd/"):
    print "error: ", str([pwd[0:17]])
    sys.exit(1)

dst = v + "envwwwt/" + fn[17:]
dst = os.path.abspath(dst)

print "ds", dst

cmd = "cp -v " + fn + " " + dst
a = os.system(cmd)
if a != 0:
    print "Error: command failed: ", cmd

print cmd, "SUCCESS"
