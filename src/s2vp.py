import os
import sys
args = sys.argv

def err(m):
    print('Error:', m); sys.exit(1)

def run(c):
    print(c)
    return os.system(c)

if len(args) < 2:
    err("s2vp.py [pattern e.g. *.bin]")

ptn = args[1]
if len(ptn.split("*")) < 2:
    ptn = '*' + ptn

cmd = 'find ./ -name "' + ptn + '" | s2s | xargs -I ARG sh -c "imv ARG;"'
print(cmd)
