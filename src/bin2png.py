import os
import sys

files = [x.strip() for x in os.popen("ls -1 *.bin").readlines()]

cmds = []
for f in files:
    cmd = "/home/" + os.popen("whoami").read().strip() + "/GitHub/bcws-psu-research/py/read_multi.py " + f + " 1" 
    cmds.append(cmd)

open("bin2png.sh", "wb").write(('\n'.join(cmds)).encode())

a = os.system("multicore ./bin2png.sh")
