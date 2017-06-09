#!/usr/bin/env python
import os
import sys
from fl0wer import error, normpath, read_config
args = sys.argv
if len(args) < 3:
    msg = 'absimag.py: take magnitudes of complex (ENVI type 6) images'
    error(msg + '\n\tuse: absimag [input path] [vertical multilook factor]')

cwd, multi_look = normpath(args[1]), int(args[2])

# read PolSARPro config.txt file
nlin, ncol = read_config(cwd + 'config.txt')

# look for 'bin' files
bins = os.popen('ls -1 ' + cwd + '*.bin').readlines()

for b in bins:
    b = normpath(b)
    fn = b.strip().split("/")[-1]
    datatype = 4
    if fn[0] == 'T':
        datatype = 4
    elif fn[0] == 's' & (fn[1] == '1' | fn[1] == '2'):
        # complex scattering matrix data (polsarpro format)
        datatype = 6
    elif (fn[0] == 'R' | fn[0] == 'L') & (fn[1] == 'R' | fn[1] == 'L'):
        # circular polarization complex data written by circ.cpp
        datatype = 6
    else:
        pass
    if datatype == 6:
        cmd = cmd = 'abs ' + str(nlin) + ' ' + str(ncol) + ' ' + b
        if multi_look != 1:
            cmd += ' ' + str(multi_look)
        run(cmd)
