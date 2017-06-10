#!/usr/bin/env python
import os
import sys
from fl0w3r import read_config
'''takes a list of folder names (rs2folders.txt) and runs the boxcar filter (C4 data) on each folder

*** note: this program isn't tested yet (need to add boxcar_filter_C4.exe to the system'''

box = "~/src/psp/420/Soft/speckle_filter/boxcar_filter_C4.exe"
nwin = 5

for fn in open("rs2folders.txt").readlines():
	f = fn.strip()
	f = os.path.abspath(f)
	c4 = f + "/C4b4box/"
	cfg = c4 + "config.txt"
	nrow, ncol = read_config(cfg)
	newc4 = f + "/C4/"
	run("mkdir -p " + newc4)
	run("cp " + cfg + " " + newc4)
	run(box + " " + c4 + " " + newc4 + " 1 " + str(nwin) + " 0 0 " + str(nrow) + " " + str(ncol))
	run("eh " + newc4)
