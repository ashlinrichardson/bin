import os
import sys

print("open all rgb.bin files in subdirectories, and display them one by one using cmd-line utility: imv")

files = os.popen('find ./ -name "rgb.bin"').readlines()

for f in files:
    f = f.strip()
    a = os.system("im " + f)


