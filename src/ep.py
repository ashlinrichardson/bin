#!/usr/bin/env python
import os
import sys
from envi import * 
from misc import error, run, args, normpath
'''reimplemented 20170622'''

def findfilestartstring(startstring, cwd):
    cmdtorun = 'find ' + cwd + ' -name ' + startstring + '*'
    # "VOL*"')
    _if = os.popen(cmdtorun).readlines()
    if len(_if) > 1:
        print("Error: more than one VOL file found in directory " + cwd)
        for lines in _if:
            print(lines.strip())
        sys.exit(1)
    else:
        _if = _if[0].strip()
    _if = os.path.abspath(_if)
    return _if

# Script Start
if len(args) < 3:
        print("ep.py: extract Palsar data 200909 reimpl. 20170622")
        error("Usage: ep [input directory] [output directory]")

sep = os.path.sep
i_dir = os.path.abspath(args[1]) + sep
o_dir = os.path.abspath(args[2]) + sep

cfgfile = i_dir + "alos_config.txt"
print("cfgfile:", str([cfgfile]))
f = open(cfgfile, "w")
f.close()

hh = findfilestartstring("IMG-HH", i_dir)
hv = findfilestartstring("IMG-HV", i_dir)
vh = findfilestartstring("IMG-VH", i_dir)
vv = findfilestartstring("IMG-VV", i_dir)
vol = findfilestartstring("VOL", i_dir)
led = findfilestartstring("LED", i_dir)
trl = findfilestartstring("TRL", i_dir)

# Get header information from Palsar PRODUCT
# aloshdr - DirOutput LeaderFile ImageFile TrailerFile ConfigFile
run("aloshdr " + i_dir + " " + led + " " + hh + " " + trl + " " + cfgfile)
lines = open(i_dir + "alos_config.txt").readlines()

# Some conversion defaults
NCOL = NROW = 0
OffsetL = 0  # No multilook (Full resolution)
OffsetC = 0
EndL = EndC = 0
RG = AZ = 1
SYM = 0  # Keep all data

for l in lines:
    l = l.strip()
    if len(l.split("nlig")) > 1:
        NROW = lines[1]
    if len(l.split("ncol")) > 1:
        NCOL = lines[4]

NROW = int(NROW)
NCOL = int(NCOL)
EndL = NROW
EndC = NCOL

'''Extract s2 coherency matrix from ALOS CEOS

TYPE: alos_convert_11_S2 FileInput11 FileInput12 FileInput21 FileInput22
  DirOutput Ncol OffsetLig OffsetCol FinalNlig FinalNcol
  SubSampRG SubSampAZ Symmetrisation (0/1) ConfigFile'''

run("mkdir " + "-p " + o_dir + "s2")
run("alos2s2 " + hh + " " + hv + " " + vh + " " + vv + " " + o_dir + "s2" +
    " " + str(NCOL) + " " + str(OffsetL) + " " + str(OffsetC) + " " +
    str(EndL) + " " + str(EndC) + " " + str(RG) + " " + str(AZ) + " " +
    str(SYM) + " " + cfgfile)

# Write some ENVI headers
write_envi_hdr(o_dir + "s2/" + "s11.bin", NCOL, NROW)
write_envi_hdr(o_dir + "s2/" + "s12.bin", NCOL, NROW)
write_envi_hdr(o_dir + "s2/" + "s21.bin", NCOL, NROW)
write_envi_hdr(o_dir + "s2/" + "s22.bin", NCOL, NROW)
