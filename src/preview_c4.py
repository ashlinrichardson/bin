#!/usr/bin/env python
'''preview for c4 data in time series
date unknown; revised 20170826'''
import os

import sys
MLKFACTOR = 4


def readRowCol(hf):
    f = open(hf)
    lines = f.readlines()
    f.close()
    samples = lins = 0
    nrow = ncol = 0
    for line in lines:
        l = line.strip()
        ls = l.split("samples")
        lg = l.split("lines")
        if(len(ls) > 1):
            ll = l.split("=")
            samples = ll[1].strip()
        if(len(lg) > 1):
            ll = l.split("=")
            lins = ll[1].strip()
    nrow = int(lins)
    ncol = int(samples)
    return([nrow, ncol])

print("indexS2mlkC4.py: front end to (indexS2mlkC4) " +
      "indexS2mlkC4.cpp (by Ashlin Richardson, September 3, 2013.\n")
print("  Using index.bin in each RS2 folder (envi type 3) " +
      "to apply multilook to Scattering matrix.")
print("Reading file:")
print("\t" + os.path.abspath("index.hdr"))
[nrow, ncol] = readRowCol(os.path.abspath("index.hdr"))
print("\tNROW " + str(nrow) + " NCOL " + str(ncol))
lines = os.popen('find ./ -name "RS2*SLC"').readlines()
print("  Looking in the following radarsat2 folders:")
for line in lines:
    print "\t" + line.strip()

cd = os.getcwd()
print("cd " + cd)

for line in lines:
    f = os.path.abspath(line.strip())
    # abs path to radarsat2 folder # #print(f)
    c4dir = f + "/s2/C4/"
    [nr, nc] = readRowCol(c4dir + "/C11.bin.hdr")
    cmd = (" cat C11.bin C22.bin C33.bin > t; imv t " +
           str(nr) + " " + str(nc) + " 3;")
    a = os.system("cd " + c4dir + "; " + cmd + " cd "+cd)
