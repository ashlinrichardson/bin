import os
import sys

files = [x.strip() for x in os.popen("ls -1 *.bin").readlines()]

cmds = []
for f in files:
    cmd = "read_multi " + f + " 1" 
    cmds.append(cmd)

open("bin2png.sh", "wb").write(('\n'.join(cmds)).encode())

a = os.system("multicore ./bin2png.sh")

