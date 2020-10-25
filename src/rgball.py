import os
import sys

files = os.popen('find ./ -name "rgb.bin"').readlines()

for f in files:
    f = f.strip()
    a = os.system("im " + f)


