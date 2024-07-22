#!/usr/bin/env python
import os
import sys
from misc import args, error, normpath

if len(args) < 4:
    error("mla.py: multilook all binary files in directory." +
          "Usage: mla [input path] [output path] [window size]")

inp, out = normpath(args[1]), normpath(args[2])
win = int(args[3])

result = os.popen("find "+inp+" -name \"*.bin\"").read()
'''Usage: multilook [input file] [path to directory containing config.txt file]
[output file] [window size]'''

result = result.strip().split()

for infile in result:
    fn = normpath(infile)
    run("multilook " + infile + " " + inp + "  " + out + " " + fn + " " +
        str(win))
