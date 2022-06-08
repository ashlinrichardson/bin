#!/usr/bin/python
import os
import sys
import array
from fl0w3r import error, run, normpath, args

if len(args) < 3:
    error("pos_v5.py: POS Polarization Orientation Shifts Correction T3" +
           " Version, Impl by A. Richardson 200712, revised 20170628" +
           "Usage: p3 [in_dir] [out_dir]")


def i(i, j):
    return 0 * i + j

t3_indir = normpath(args[1])
t3_outdir = normpath(args[2])

extract_chnl, pos_pal_cmd = "p3ec", "p3c"
run("cp " + t3_indir + "config.txt " + t3_outdir + "config.txt")

print("-------------------------------------------------------------------")
print("STEP 1")
print(pos_pal_cmd)
print("-------------------------------------------------------------------")
run(pos_pal_cmd+" "+t3_indir+" "+t3_outdir)
run("cp "+t3_indir+"config.txt "+t3_outdir+"config.txt")

print("-------------------------------------------------------------------")
print("STEP 2")
print(extract_chnl)
print("-------------------------------------------------------------------")
run(extract_chnl+" "+t3_indir+" "+t3_outdir)
run("mv -v theta " + t3_outdir)

print("-------------------------------------------------------------------")
print("OUTPUT FILES")
print(t3_indir)
print(t3_outdir)
print("-------------------------------------------------------------------")
