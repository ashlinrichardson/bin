'''reimplemented 20170623'''
import os
import sys
from envi import read_envi_hdr2
from fl0w3r import normpath, run, error, args
'''Usage: cvn [input file name] [input data type] [samples] [lines] [bands]
 [output file name] [output data type]'''

if len(args) < 4:
    error("ffn.py: Force fit a BSQ floating point (ENVI data type 4) image." +
          "Updated by Ash Richardson, June 2009; 20170622. \tUsage: ff " +
          "[input file name] [output file name] [force fit param file name]" +
          "Note that the header file must be present as [input file name].hdr")

infile, outfile = normpath(args[1]), normpath(args[2])
ffparamfile = normpath(args[3])

inphdr = infile + ".hdr"
outhdr = outfile + ".hdr"

lines, samples, bands = read_envi_hdr(inphdr)
os.system("cp "+inphdr+" "+outhdr)

d = read_envi_hdr2(inphdr)

samples, lines, bands, dt = d['lines'], d['samples'], d['bands'],\
                            d['data type']

print samples, lines, band, dt

if(samples * lines * bands == 0):
    error("reading samples lines or bands from header file.\n")

run("fff " + infile + " " + str(samples) + " " + str(lines) + " " +
    str(bands) + " " + ffparamfile)
