'''add prefix to files within present folder 20220529'''
import os
import sys
import shutil
args = sys.argv

def err(m):
    print("Error:", m); sys.exit(1)

if len(args) < 2:
    err("files_prefix.py [prefix to add to files in present dir]")

pre = args[1].replace(' ', '_')

files = [x.strip() for x in os.popen("ls -1").readlines()]
for f in files:
    if os.path.exists(f) and os.path.isfile(f):
        print(pre + f)

        shutil.move(f, pre + f)
