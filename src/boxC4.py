#!/usr/bin/env python
import os
import sys
from fl0w3r import run, normpath, chkfile
from envi import read_envi_hdr

filter_size = 3

chkfile("index.hdr")
print("boxC4.py: front end to box_filter_C4.cpp  (by Ashlin Richardson, 20130903 revisioned 20170609\n");
print("\tUsing index.bin in each RS2 folder (envi type 3) to apply multilook to Scattering matrix.");
print("\tReading file:")
print("\t\t" + os.path.abspath("index.hdr"))

[nrow, ncol] = read_envi_hdr("index.hdr"))
lines = os.popen('find ./ -name "RS2*SLC"').readlines()

print("  Looking in the following radarsat2 folders:")
for line in lines:
	print "\t" + line.strip()

cd = os.getcwd()
print("cd " + cd)

for line in lines:
	f = normpath(line.strip())
	c4dir = f + "s2/C4/"
	odir = f + "s2/C4box/"
	nr, nc = read_envi_hdr(c4dir + "C11.bin.hdr")
  run("mkdir -p " + odir)
  run("boxcar_filter_C4.exe "+ c4dir + " " + odir + " 1 " + str(filter_size) + " 0 0 " + str(nr) + " " + str(nc))