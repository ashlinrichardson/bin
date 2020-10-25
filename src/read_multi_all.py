import os
import sys

files = os.popen('find ./ -name "rgb.bin"').readlines()

py = '~/GitHub/bcws-psu-research/py/read_multi.py'


for f in files:
    f = f.strip()
    a = os.system(py + ' ' + f + ' 1')

