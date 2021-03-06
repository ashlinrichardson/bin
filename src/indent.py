# add four spaces to the start of each line. Remove right trailing whitespace
import os
import sys

lines = [line.rstrip() for line in open(sys.argv[1]).readlines()]
os.copy(sys.argv[1], sys.argv[1] + ".bak")

open(sys.argv[1], 'wb').write(('\n'.join(['    ' + line for line in lines])).encode())
