#!/usr/bin/env python
import os
import sys
from fl0w3r import error, run
'''reimplemented 20170617

program based on cv.c:

Usage: cv [input file name] [input data type] [samples]
          [lines] [bands] [output file name] [output data type]
'''
args = sys.argv
if len(args) != 4:
    msg = "cv.py: Convert between ENVI data types.  Updated 200905 by AR."
    msg += "\nUsage: cv [input ile name] [outfile name] [output data type]"
    error(msg + "\nheader file called [input file name].hdr needed")

infile, outfile = args[1], args[2]
outtype = int(float(args[3]))
inphdr, outhdr = infile + ".hdr", outfile + ".hdr"

flines = open(inphdr).readlines()

# open output file
g = open(outhdr, "w")

samples, lines, bands, old = 0, 0, 0, 0
for _l in flines:
    line = _l.strip()

    if(len(line.split("samples")) == 2):
        ll = line.split("samples")[1].strip().strip("=")
        samples = int(ll)

    if(len(line.split("lines")) == 2):
        ll = line.split("lines")[1].strip().strip("=")
        lines = int(ll)

    if(len(line.split("bands")) == 2):
        ll = line.split("bands")[1].strip().strip("=")
        bands = int(ll)

    if(len(line.split("data type")) == 2):
        # is data type line
        ll = line.split("data type")[1].strip().strip("=")
        old = int(ll[1])
        g.write("data type = " + str(outtype) + "\n")
    else:
        # is some other kind of line (any kind)
        g.write(line + "\n")

g.close()

if(samples * lines * bands == 0):
    error("Error reading samples lines or bands from header file.\n")

run("cv " + infile + " " + str(old) + " " + str(samples) + " " + str(lines) +
    " " + str(bands) + " " + outfile + " " + str(outtype))
