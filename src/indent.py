# add four spaces to the start of each line. Remove right trailing whitespace
import os
import sys
import shutil

lines = [line.rstrip() for line in open(sys.argv[1]).readlines()]
shutil.copyfile(sys.argv[1], sys.argv[1] + ".bak")

open(sys.argv[1], 'wb').write(('\n'.join(['    ' + line for line in lines])).encode())
