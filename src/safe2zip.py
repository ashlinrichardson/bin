'''take folders ending in .SAFE in a directory and create zip files from them

replacing .SAFE with .zip. Written 20210718'''
import os
import sys
import shutil

def err(m):
    print("Error: " + str(m)); sys.exit(1)

to_zip = []
files = [f.strip() for f in os.popen("ls -1").readlines()]
for f in files:
    if os.path.isdir(f):
        if f.split('.')[-1] == 'SAFE':
            to_zip.append(f)

for f in to_zip:
    target = '.'.join(f.strip().split('.')[:-1] + ['zip']) # replace .SAFE with .zip for zipfile..
    shutil.make_archive(target, 'zip', f)
