#!/usr/bin/env python
'''search for files matching pattern and sort by size 20170626'''
import os
import sys
from fl0w3r import args, normpath, error


def msg():
    print("Usage:\n\tlss.py [path] [pattern]\n")
    print("\t\t'pattern' refers to files (not directories)")
    print("that match the pattern.\n\tlss.py [path]\n\t\t..")
    print("this usage matches all kinds of files..")
    sys.exit(1)

pattern = ''

if len(args) < 2:
    msg()

pwd = normpath(args[1])
if (not os.path.exists(pwd)) or (not os.path.isdir(pwd)):
    msg()

if len(args) > 2:
    pattern = args[2].strip()

# findParams:
cmd = "find " + pwd + ' -name "' + pattern + '"'
if len(args) <= 2:
    cmd = "find " + pwd

lines = os.popen(cmd).read().strip().split("\n")

a = []
for line in lines:
    l = line.strip()
    try:
        if(not(os.path.isdir(l)) and os.path.isfile(l)):
            a.append([(os.stat(l)).st_size, l])
    except:
        error(str([l]))

a.sort(key=lambda x: x[0])

for line in a:
    print(str(line[0]) + " " + line[1])

print("next step would be.. bin by fns, sizes?..")
