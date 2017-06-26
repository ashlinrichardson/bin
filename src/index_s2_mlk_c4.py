#!/usr/bin/env python
import os
import sys
from fl0w3r import args, run, error
from envi import read_envi_hdr

if len(args) < 2:
    error("indexS2mlkC4py.py [multilook factor (square)]" +
          "front end to (indexS2mlkC4) indexS2mlkC4.cpp " +
          " (by Ashlin Richardson, September 3, 2013.\n" +
          "using index.bin in each RS2 folder (envi type 3)" +
          "to apply multilook to Scattering matrix. Searches" +
          'for directories according to find ./ -name "RS2*SLC"' +
          " and runs the c++ program in all of these directories." +
          "Check source code for output location(s).")

mlk_factor = int(args[1])

''' indexS2mlkC4.py: front end to (indexS2mlkC4) indexS2mlkC4.cpp (by
Ashlin Richardson, Sep. 3, 2013. Reimpl 20170626. Using index.bin in
each RS2 folder (envi type 3) to apply multilook to Scattering matrix.")'''

nrow, ncol = read_envi_hdr(normpath('index.hdr'))
print("\tNROW "+str(nrow)+" NCOL "+str(ncol))

lines = os.popen('find ./ -name "RS2*SLC"').readlines()
print("  Looking in the following radarsat2 folders:")
for line in lines:
    print "\t"+line.strip()

cd = os.getcwd()
print("cd " + cd)

for line in lines:
    f = os.path.abspath(line.strip())
    s2dir = os.path.abspath(f + "/s2/")
    c4dir = s2dir + "/" + "mlk" + str(mlk_factor) + "C4/"
    a = os.system("mkdir " + c4dir)
    hdr = s2dir + "/s11.bin.hdr"
    cfg = s2dir + "/../config.txt"  # a = os.system("ls -latrh "+hdr)
    r, c = read_envi_hdr(hdr)
    print(hdr + " NROW " + str(r) + " NCOL " + str(c))
    print("s2dir " + s2dir)
    cmd = "eh2cfg " + hdr + " " + cfg
    a = os.system(cmd)
    cmd = ("indexS2mlkC4 " + f + "/index.bin " + str(nrow) + " " + str(ncol) +
           " " + str(r) + " " + str(c) + " " + str(mlk_factor) + " " +
           str(MLKFACTOR) + " " + c4dir + " " + c4dir + "/mask.bin")
    a = os.system("cd " + s2dir + "; " + cmd + "; cd " + cd)
