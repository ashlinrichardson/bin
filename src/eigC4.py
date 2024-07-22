#!/usr/bin/env python
import os
import sys
from misc import error
from envi import read_envi_hdr

mybindir = "~/psp/420/Soft/data_process_sngl/"
FILTERSIZE = 3
# MLKFACTOR = 4

print("This script unfinished, based on boxC4.py: front end to box_filter_C4.cpp  (by Ashlin Richardson, September 3, 2013. revisited 20160622\n");
print("Using index.bin in each RS2 folder (envi type 3) to apply multilook to Scattering matrix.");
print("the index.bin came from coregistering an index file, instead of the actual data...\n");
print("some lines are commented out: not sure what this script is supposed to do (look at it again later)\n")
print("Reading file:")
print("\t"+os.path.abspath("index.hdr"))
[nrow, ncol] = readRowCol(os.path.abspath("index.hdr"));
print("\tNROW "+str(nrow)+" NCOL "+str(ncol))

lines = os.popen('find ./ -name "RS2*SLC"').readlines();
print("  Looking in the following radarsat2 folders:");
for line in lines:
	print "\t"+line.strip();

cd = os.getcwd();
print("cd "+cd)

for line in lines:
	f = normpath(line.strip()) # abs path to radarsat2 folder # #print(f)
	c4dir = f + "/s2/C4/"
	odir = f + "/s2/C4box/"
	[nr, nc] = readRowCol(c4dir + "/C11.bin.hdr")

	# cmd = "boxcar_filter_C4.exe "+ c4dir + " " +odir+ " 1 "+str(FILTERSIZE)+" 0 0 "+str(nr)+" "+str(nc);
	# cmd = mybindir+"h_a_alpha_decomposition_C4.exe ./ ./ 1 0 0 "   +str(nr)+" "+str(nc)+ "  1 1 1 1 1 1 1 1 1 ";
	# print(cmd);
	# a = os.system("cd "+odir+"; "+cmd);
	# cmd = mybindir+"h_a_alpha_eigenvalue_set_C4.exe ./ ./  1 0 0 " +str(nr)+" "+str(nc)+ " 1 1 1 1 1 1 1 1 1 1 1 1 ";
	# print(cmd);
	# a = os.system("cd "+odir+"; "+cmd);
	# cmd = mybindir+"h_a_alpha_eigenvector_set_C4.exe ./ ./ 1 0 0 " +str(nr)+" "+str(nc)+ " 1 1 1 1 1 1 1 ";
	# print(cmd);
	# a = os.system("cd "+odir+"; "+cmd);

	cmd = "cd " + odir + "; " + " correctC4 ./ " + str(nr) + " " + str(nc) + " ./"
	print(cmd)
	a = os.system(cmd)
	#a = os.system("cd "+c4dir+"; "+cmd+" cd "+cd)


