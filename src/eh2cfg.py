#!/usr/bin/env python
import os
import sys
from envi import read_envi_hdr
from fl0w3r import args, write_config, error

if len(args) < 3:
	error('eh2cfg.py: read lines/samples from envi header file, output to' + 
        ' polsarpro config.txt file ' +
	      'Usage: [envi header file name] [path to polsarpro config file')

# read envi header
nlin, ncol, nband = read_envi_hdr(args[1])

# write image dimensions to PolSARPRo config file
write_config(args[2], nlin, ncol)
