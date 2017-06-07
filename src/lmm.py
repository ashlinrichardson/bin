#!/usr/bin/env python
'''want to know where your disk space is used?
summarize file space usage, producing a grand total (du -c), with sorting
'''
import os
import sys
from fl0w3r import error, normpath, exists
args, pwd = sys.argv, normpath(os.getcwd())
print pwd
if len(args) > 1:
    pwd = normpath(args[1])
    if not exists(pwd) or not os.path.isdir(pwd):
        error('lmm.py: use: lmm.py or lmm.py [path] (defaults to pwd)')
cmd = "du -c " + pwd
a = os.popen(cmd).read().strip().split('\n')
lines = []
for line in a:
    ls = line.strip().split()
    if len(ls) > 2:
        error('paths with spaces not supported')
    lines.append([int(ls[0]), normpath(ls[1])])
lines.sort(key=lambda x: x[0])

for line in lines:
    print str(line[0]) + ' ' + line[1]
