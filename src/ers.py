#!/usr/bin/env python
import os
import re
import sys
from os.path import *
from fl0w3r import error, chkdir, chkfile, normpath, run, require_gdal, wopen

require_gdal()

args = sys.argv
if len(sys.argv) < 3:
    msg = 'ers.py: Extract Radarsat-2 data to ENVI format using GDAL\n'
    msg += 'Usage: ers [rsat2 data dir] [output dir]\n'
    error(msg + 'reimplemented 20170605 from 200809 script')

in_dir, out_dir = args[1], args[2]

if not chkdir(in_dir):
    error('invalid input directory: ' + in_dir)

if not chkdir(out_dir):
    error('output folder does not exist: ' + out_dir)

in_dir, out_dir = normpath(in_dir), normpath(out_dir)

# read rsat-2 header
run('irs ' + in_dir)

# copy product.xml and config.txt files
run('cp ' + in_dir + 'config.txt ' + out_dir)
run('cp ' + in_dir + 'product.xml ' + out_dir)

sf = {'s11': 'HH', 's12': 'HV', 's21': 'VH', 's22': 'VV'}

for s in sf:
    # extract the data with gdal_translate
    run('gdal_translate -of ENVI -ot Float32 -co INTERLEAVE=BIP ' +
        in_dir + 'imagery_' + sf[s] + '.tif ' + out_dir + s + '.bin')

    # adjust header file
    hfn = out_dir + s + '.hdr'
    chkfile(hfn)
    hd = open(hfn).read()
    hd = hd.replace('description = {\n', 'description = {')
    hd = hd.replace('bands   = 2', 'bands   = 1')
    hd = hd.replace('data type = 4', 'data type = 6')
    hd = hd.replace('interleave = bip', 'interleave = bsq')
    hd = hd.replace('\nband names = {\nBand 1,\nBand 2}', '')
    wopen(hfn).write(hd)
