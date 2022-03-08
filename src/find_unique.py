'''20220307 Group results of find command, into equivalence classes
(by file name not incl. path)

Optional argument: whether to display the locations where each filename exist
(could also do this by piping into a "unique" command..
..next time actually should implement unique.cpp to take stdin for unix pipe)

Usage:
    python3 find_unique.py [pattern e.g. PRS*zip for prisma data]'''
import os
import sys

def err(m):
    print('Error:', m); sys.exit(1)

def msg():
    err('Group results of find command, into equivalence classes.' +
        'Usage:\n\tfind_unique [pattern e.g. PRS*zip for prisma data] ' +
        '[1: optional arg: display paths in equivalence class (put 1 here)]')

args = sys.argv
if len(args) < 2 or len(args) > 3:
    msg()

if len(args) > 2:
    if args[2] != '1':
        msg()

sep = os.path.sep
cmd = 'find ./ -name "' + args[1] + '"'
lines = [x.strip() for x in os.popen(cmd).read().strip().split('\n')]

x = {}
for line in lines:
    fn = os.path.abspath(line).split(sep)[-1]
    if not fn in x:
        x[fn] = []
    x[fn].append(line)

show_paths = len(args) > 2 and args[2] == '1'
x_keys = [i for i in x]
x_keys.sort()
for i in x_keys:
    print(str(i) + ' x' + str(len(x[i])))
    if show_paths:
        for j in x[i]:
            print('\t', j)
