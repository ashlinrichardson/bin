# 20230802 replace pattern in filenames with a different string

import os
import sys
args = sys.argv

def err(m):
    print("Error:", m); sys.exit(1)

def run(c):
    print(c)
    return os.system(c)


cmd = 'find ./ -name "*' + args[1] + '*"'
print(cmd)
lines = [x.strip() for x in os.popen(cmd).readlines()]

for line in lines:
    print(line)

cmds = []
for line in lines:
    subs = line.split(os.path.sep)[-1]
    repl = subs.replace(args[1], args[2])
    
    rlin = line.replace(subs, repl)
    
    cmd = 'cp ' + line + ' ' + rlin
    print(cmd)
    cmds += [cmd]


inp = input('proceed? (Y/N)').lower()

if inp == 'y':
    for cmd in cmds:
        run(cmd)

