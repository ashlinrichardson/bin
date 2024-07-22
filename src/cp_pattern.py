# 20230731 copy files matching pattern to specified directory

import os
import sys
args = sys.argv

def err(m):
    print("Error:", m); sys.exit(1)

def run(c):
    print(c)
    return os.system(c)


lines = [x.strip() for x in os.popen('find ./ -name "' + args[1] + '"').readlines()]


cmds = []
for line in lines:
    cmd = 'cp ' + line + ' ' + args[2]
    print(cmd)
    cmds += [cmd]


inp = input('proceed? (Y/N)').lower()

if inp == 'y':
    for cmd in cmds:
        run(cmd)

