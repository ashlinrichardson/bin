#!/usr/bin/env python
'''convert radarsat 2 data to S2 format,
 then apply a Shane Cloude methodology to it.'''
import os
import sys
from fl0w3r import args, error, run

if len(args) < 2:
    error("rbp.py: Process Radarsat-2 Data from unzipped files to a Shane " +
          "Cloude imagery.\nUsage: rpb [batch file]\nSee wiki page for " +
          "example batch file.")

fn = args[1]

lines = open(fn).readlines()

for l in lines:
    w_d = l.strip()
    print "processing", w_d
    cmd = "gdal_translate -of ENVI -ot Float32 -co INTERLEAVE=BIP "
    run(cmd + w_d + "imagery_HH.tif " + w_d + "s11.bin")
    run(cmd + w_d + "imagery_HV.tif " + w_d + "s12.bin")
    run(cmd + w_d + "imagery_VH.tif " + w_d + "s21.bin")
    run(cmd + w_d + "imagery_VV.tif " + w_d + "s22.bin")
    run("mkdir " + w_d + "S2/")
    run("mv " + w_d + "*.bin " + w_d + "S2/")
    run("irs " + w_d)
    run("cp " + w_d + "config.txt " + w_d + "S2/config.txt")
    run("scm " + w_d + "S2/ " + w_d + "S2_scm/ box 5 2")
