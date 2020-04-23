import os
import sys
args = sys.argv
msg = "update"

def err(m):
    print("Error: " + str(m)); sys.exit(1)

def run(c):
    print('[' + c + ']')
    a = os.system(c)
    if a != 0: err("command failed: [" + c + "]")

if len(args) < 2:
    err("Usage: save [file name] # add an extra parameter that's not a file, to change the commit msg")

fns = args[1:]
for fn in fns:
    if not os.path.exists(fn):
        # err("Error: file not found: " + fn)
        msg = fn.replace('"', ' ')
    run('git add ' + fn)

run('git commit -m "'  + msg + '"')
run('git push origin master')
