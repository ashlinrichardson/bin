#!/usr/bin/env python

import os,sys

if len(sys.argv) < 2:
	print "Usage: [masterI]"
	sys.exit(1)

masterI = int(sys.argv[1])

i = 1
for fn in open("rs2folders.txt").readlines():
	f = fn.strip()
	infolder = f + "/C4/C4_coreg/"
	outfolder = "C4_" + str(i)
	outfolder = os.path.abspath(outfolder)
	if(i == masterI):  # for the master, use the "non-coregistered" folder
		infolder = f + "/C4/"
	cmd = "ln -s  " + infolder + " " + outfolder  # 20130218/C2/C2_coreg/ C2_1"
	#print cmd
	a = os.system(cmd)
	print a, cmd
	i += 1
