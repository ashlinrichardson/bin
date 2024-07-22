#!/usr/bin/env python
import os
import sys
from misc import error, normpath, run

args = sys.argv
if len(args) < 2:
  error("btl.py: Convert floating point type HSV binary files into integer type (scaled) e.g., for use with Logit.\nNote that a radar config.txt file must be present.\nUsage: btl [directory path]\n")

cwd = normpath(args[1])
NRow, NCol = read_config(cwd + "config.txt")

run("btlsf " + cwd + "Hscaled.bin " + str(NRow) + " " + str(NCol) + " " + cwd + "Hlogit.bin")
run("btlsf " + cwd + "Sscaled.bin " + str(NRow) + " " + str(NCol) + " " + cwd + "Slogit.bin")
run("btlsf " + cwd + "Vscaled.bin " + str(NRow) + " " + str(NCol) + " " + cwd + "Vlogit.bin")
