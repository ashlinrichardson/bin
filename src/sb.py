#!/usr/bin/env python
import os
import sys
from misc import args, run, normpath, error, read_config
from envi import write_envi_hdr

if len(args) < 2:
    error("sb.py: Scale 32 bit binary files into range 0-1.  Req. ENVI data" +
          "type 4, single band, with config.txt file present.\n" +
          "Usage: sb [input directory].")

cwd = normpath(args[1])
config_file = cwd + "config.txt"

NRow, NCol = read_config(config_file)

bins = os.popen('ls -1 ' + cwd + '*.bin').read().strip().split('\n')

for b in bins:
    '''need to check the names here'''
    ofname = b[:-4] + "scaled.bin"
    os.system("btfsf " + b + " " + str(NRow) + " " + str(NCol) + " " + ofname)
    write_envi_hdr(ofname + '.hdr', NRow, NCol)
