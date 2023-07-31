import os
import sys

def run(c):
    print(c); return os.system(c)

lines = [x.strip() for x in os.popen("ls -1 *.heic").readlines()]

for line in lines:
    cmd = 'heif-convert ' + line + ' ' + line[:-5] + '.jpg'
    run(cmd)

