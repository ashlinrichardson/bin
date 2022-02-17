#!/usr/bin/env python
import os
import sys
from fl0w3r import error, run
'''reimplemented 20170617

program based on cv.c:

Usage: cv [input file name] [input data type] [samples]
          [lines] [bands] [output file name] [output data type] '''
args = sys.argv
if len(args) != 4:
    msg = "cv.py: Convert between ENVI data types.  Updated 200905 by AR."
    msg += "\nUsage: cv [input ile name] [outfile name] [output data type]"
    error(msg + "\nheader file called [input file name].hdr needed")

infile, outfile = args[1], args[2]
outtype = int(float(args[3]))
inphdr, outhdr = infile[:-4] + ".hdr", outfile[:-4] + ".hdr"
print("+r", inphdr)
flines = open(inphdr).readlines()

g = open(outhdr, "w")  # open output file
samples, lines, bands, old = 0, 0, 0, 0
for _l in flines:
    line = _l.strip()
    w = [x.strip() for x in line.split("=")]
    if w[0] == "samples": samples = int(w[1])
    if w[0] == "lines": lines = int(w[1])
    if w[0] == "bands": bands = int(w[1])
    if w[0] == "data type":
        old = int(w[1])
        g.write("data type = " + str(outtype) + "\n")
    else:
        g.write(line + "\n")
g.close()
print("Convert from type:", old)      

if(samples * lines * bands == 0):
    error("Error reading samples lines or bands from header file.\n")

run("cv " + infile + " " + str(old) + " " + str(samples) + " " + str(lines) +
    " " + str(bands) + " " + outfile + " " + str(outtype))
