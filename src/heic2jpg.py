import os
import sys

lines = [x.strip() for x in os.popen("ls -1 *.heic").readlines()]

for line in lines:
    cmd = 'heif-convert ' + line + ' ' + line[:-4] + '.jpg'
    print(cmd)

