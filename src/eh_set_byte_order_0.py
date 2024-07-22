#!/usr/bin/env python
'''description: write envi headers for type-4 envi files in a directory with
config.txt present. date: march 30, 2012 reimplemented 20170621 arichardson'''

import os
import sys
from envi import write_envi_hdr
from misc import run, normpath, error, args, read_config, exists

if len(args) < 2:
    error('Usage: eh [input directory]')

cwd = normpath(args[1])

# read image dimensions from config file
# config_file = cwd + 'config.txt'
# nlin, ncol = read_config(config_file)

# list a files in cwd with filenames: *.bin 
hdrs = os.popen('ls -1 ' + cwd + '*.hdr').read().strip().split('\n')

# go through all of the files
for hfn in hdrs:
    lines = [x.rstrip() for x in open(hfn).readlines()]
    for i in range(len(lines)):
        line = lines[i]
        w = [x.strip() for x in line.split()]
        if w[0] == 'byte' and w[1] == 'order':
            lines[i] = lines[i].replace('1', '0')
            print(line)
    open(hfn, 'wb').write(('\n'.join(lines)).encode())
