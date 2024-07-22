#!/usr/bin/env python
import os
import re
import sys
from misc import normpath, read_config

args = sys.argv
if len(args) < 2:
	print "btf.py: Convert floating point type HSV binary files into float in range 0-1."
	print "Note that a radar config.txt file must be present.\nUsage: btf [directory path]"
	print "200906 revisited 20170609"
	sys.exit(1)

cwd = normpath(args[1])
config_file = cwd + "config.txt"

NRow, NCol = read_config(config_file);
run("btfsf "+cwd+"Hscaled.bin "+str(NRow)+" "+str(NCol)+" "+cwd+"Hscaled0to1.bin");
run("btfsf "+cwd+"Sscaled.bin "+str(NRow)+" "+str(NCol)+" "+cwd+"Sscaled0to1.bin");
run("btfsf "+cwd+"Vscaled.bin "+str(NRow)+" "+str(NCol)+" "+cwd+"Vscaled0to1.bin");
