#!/usr/bin/env python
'''preview for C4 data
revised 20170826'''
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
      " indexS2mlkC4.cpp (by Ashlin Richardson, September 3, 2013.\n")
print("  Using index.bin in each RS2 folder (envi type 3) to apply" +
      " multilook to Scattering matrix.")
print("Reading file:")
print("\t" + os.path.abspath("index.hdr"))
[nrow, ncol] = readRowCol(os.path.abspath("index.hdr"))
print("\tNROW " + str(nrow) + " NCOL "+str(ncol))

lines = os.popen('find ./ -name "RS2*SLC"').readlines()
print("  Looking in the following radarsat2 folders:")
for line in lines:
    print "\t" + line.strip()

cd = os.getcwd()
print "cd " + cd

for line in lines:
    f = os.path.abspath(line.strip())
    # abs path to radarsat2 folder # #print(f)
    c4dir = f + "/s2/C4box/"
    cmd = "cd " + f + " /s2/; "
    cmd += " cp C4/config.txt C4box/config.txt; cd C4box; eh ./; "
    a = os.system(cmd + " cd " + cd)

    [nr, nc] = readRowCol(c4dir + "/C11.bin.hdr")
    cmd = "cd " + f + "/s2/C4box; "
    cmd += " cat C11.bin C22.bin C33.bin > t; imv t " + str(nr) + " "
    cmd += str(nc) + " 3;"
    a = os.system(cmd + " cd " + cd)
