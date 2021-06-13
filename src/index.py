#!/usr/bin/env python
'''20170626 write an "Index band to be geometrically transformed"..
..this was for radarsat2. '''

import os
import sys
from fl0w3r import args, error, run, read_config, normpath
from envi import write_envi_hdr
''' Unsigned Long: A 32-bit unsigned integer ranging in value from 0 to
4 294 967 296. AKA ENVI type: 13 '''

fn = normpath("product.xml")
nrow, ncol = 0, 0

for line in open(fn).readlines():
    l = line.strip()
    iscol = len(l.split("numberOfSamplesPerLine")) > 1
    isrow = len(l.split("numberOfLines")) > 1
    if(iscol):
        dc = l.split("numberOfSamplesPerLine")[1]
        ncol = int(dc.strip('>').split('<')[0])
    if(isrow):
        dr = l.split("numberOfLines")[1]
        nrow = int(dr.strip('>').split('<')[0])

run("ind_image " + str(nrow) + " " + str(ncol) + " " + normpath("index.bin"))

write_envi_hdr("index.hdr", nrow, ncol)
