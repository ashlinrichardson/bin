#!/usr/bin/env python3
import os
import sys

files = os.popen('find ./ -name "rgb.bin"').readlines()

py = '/home/' + os.popen('whoami').read().strip() + '/GitHub/bcws-psu-research/py/read_multi.py'
if not os.path.exists(py):
    print("Error: can't find file:", py)
    sys.exit(1)

for f in files:
    f = f.strip()
    a = os.system(py + ' ' + f + ' 1 &')
