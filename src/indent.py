'''indent text file: add four spaces to the start of each line..
.. Remove right trailing whitespace'''
import os
import sys
import shutil
args = sys.argv
f_i = args[1] # input file
f_b = args[1] + '.bak' # backup original file

print("+r", f_i)
lines = [line.rstrip() for line in open(f_i).readlines()]

print(" ".join(["cp", f_i, f_b]))
shutil.copyfile(f_i, f_b)

print("+w", f_i)
open(f_i, 'wb').write(('\n'.join(['    ' + line for line in lines])).encode())
