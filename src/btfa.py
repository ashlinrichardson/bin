#!/usr/bin/env python
import os
import sys
from misc import run, error, read_config, normpath
from envi import write_envi_hdr
args = sys.argv

if len(args) > 2:
	print "Usage: btfa [input directory]"
	print "Scale all files .bin ENVI type 4 in directory to range 0-1"
	print "Each file *.bin appears scaled as *.bin.01"
	sys.exit(1)

cwd = normpath(args[1])
nlin, ncol = read_config(cwd + "config.txt")

bins = os.popen("ls "+cwd+"*.bin").read()
bins = bins.strip()

bins = bins.replace("\n", " ")
bins = bins.strip().split(" ")
a = bins[0].split(".")

if(a[(len(a)-1)] == "bin"):
	  pass
else:
	error("no bin files in directory")

for b in bins:
	  b = abspath(realpath(normpath(b)))
	  splitdir = b.strip().split("/")
	  fn = splitdir[len(splitdir)-1]
	  datatype = 4
	  if (fn[0]=='T'):
		    datatype = 4
	  elif((fn[0]=='s') & ((fn[1]=='1')|(fn[1]=='2'))):
		    datatype = 6 
		    # complex scattering matrix data (polsarpro format).
	  elif(((fn[0]=='R') | (fn[0]=='L'))  &   ((fn[1]=='R') | (fn[1]=='L'))):
		    datatype = 6
        # circular polarization complex data written by circ.cpp.
	  else:
		    datatype = 4
	run("btfsf " + b +" " + str(nlin) + " " + str(ncol) + " " + b + "_01.bin")
