#!/usr/bin/env python
import os
import sys
from misc import error, normpath, run
args = sys.argv
if len(args) < 3:
  error("pci8toS2.py: Convert 8-channel real/imaginary int file (from PCI) into the scattering matrix standard format\nUsage: c82 [infile] [output directory path]\n")

cwd = normpath(args[2])
in_file = sys.argv[1] 

#extract the 2 bands (real and imaginary) to BIP floating point data type files.
run("gdal_translate -ot Float32 -b 1 -b 2 -of ENVI -co INTERLEAVE=BIP " + in_file + " " + cwd + "s11.bin")
run("gdal_translate -ot Float32 -b 3 -b 4 -of ENVI -co INTERLEAVE=BIP " + in_file + " " + cwd + "s12.bin")
run("gdal_translate -ot Float32 -b 5 -b 6 -of ENVI -co INTERLEAVE=BIP " + in_file + " " + cwd + "s21.bin")
run("gdal_translate -ot Float32 -b 7 -b 8 -of ENVI -co INTERLEAVE=BIP " + in_file + " " + cwd + "s22.bin")

for fn in ['s11', 's12', 's21','s22']:
	fa = fn + ".hdr"
	fi = open(cwd + fa).read();
  ''' convert the ENVI header so that it represents a complex data type.
  The binary file is not altered, just the interpretation of it by ENVI '''
	out = ((fi.replace("bands   = 2","bands   = 1")).replace("data type = 4","data type = 6")).replace("interleave = bip", "interleave = bsq")
	out = out.replace("\nband names = {\nBand 1,\nBand 2}","")
	open(cwd + fa, "w").write(out)
  
  # set the file name convention
	run("mv " + cwd + fa + " " + cwd + fn + ".bin.hdr");

