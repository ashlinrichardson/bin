#!/usr/bin/env python
'''want to know where your disk space is used?
summarize file space usage, producing a grand total (du -c), with sorting
'''
import os
import sys
from fl0w3r import error, normpath
args, pwd = sys.argv, normpath(os.getcwd())
if len(args) > 1:
    pwd = os.path.abspath(args[1])
    if not os.path.exists(pwd) or not os.path.isdir(pwd):
        error('lmm.py: use: lmm.py or lmm.py [path] (defaults to pwd)')
cmd = "du -c " + pwd
a = os.popen(cmd).read().strip().split('\n')
lines = []
for line in a:
    ls = line.strip().split()
    n = int(ls[0])
    rest = (" ".join(ls[-1:])).strip()
    lines.append([n, line])
lines.sort(key=lambda x: x[0])

for line in lines:
    print(str(line[0])+" "+line[1])
