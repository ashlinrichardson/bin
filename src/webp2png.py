import os
import sys
files = os.popen('ls -1 *.webp').readlines()
for f in files:
    f = f.strip()
    cmd = 'convert "' + f + '" "' + f[:-4] + 'png"'
    print(cmd)
    a = os.system(cmd)
    if a==0:
        a = os.system('rm "' + f + '"')
