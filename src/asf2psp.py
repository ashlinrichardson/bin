#!/usr/bin/env python
'''convert ASF sar data to PolSARPro format. Revisioned 20170609'''
import os
import sys
from envi import write_envi_hdr
from misc import error, run, write_config, chkfile

chkfile("import.hdr")

lines = open("import.hdr").readlines()
ncol, nrow, off_set, dat, inter_leave = 0, 0, 0, 0, 0

for l in lines:
    l = l.strip()
    if len(l.split("samples")) > 1:
        ncol = l.split()[2]
    if len(l.split("lines")) > 1:
        nrow = l.split()[2]
    if len(l.split("header offset")) > 1:
        off_set = l.split()[3]
    if len(l.split("data type")) > 1:
        dat = l.split()[3]
    if len(l.split("interleave")) > 1:
        inter_leave = (l.split())[2]

ncol, nrow = int(ncol), int(nrow)
if ncol * nrow == 0:
    error("Error: header had one dimension set to zero.")

off_set = int(off_set)
if off_set != 0:
    error("nonzero offset")

dat = int(dat)
if dat != 4:
    error("data type must be 4.")

if inter_leave != "bsq":
    error("data type must be bsq.")

# make it go
run("asf2pspc " + str(nrow) + " " + str(ncol) + " ")

# write PolSARPro config file
write_config("config.txt", nrow, ncol)

# write ENVI style headers
write_envi_hdr("s11.bin", ncol, nrow)
write_envi_hdr("s12.bin", ncol, nrow)
write_envi_hdr("s21.bin", ncol, nrow)
write_envi_hdr("s22.bin", ncol, nrow)
