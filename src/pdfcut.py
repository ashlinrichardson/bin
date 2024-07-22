#!/usr/bin/env python
'''this program should check for ghostscript'''
import os
import sys
from misc import error, run, exists

args = sys.argv
if(len(args) < 4):
    m = "pdfcut\nUse: [first pg (1-)] [last pg] [infile.pdf] [outfile.pdf]\n"
    error(m)

start, end, infile, outfile = int(args[1]), int(args[2]), args[3], args[4]

if not exists(infile):
    error("input file not found:" + infile)

# run ghostscript
run("gs -sDEVICE=pdfwrite -dNOPAUSE -dBATCH -dFirstPage=" + str(start) +
    " -dLastPage=" + str(end) + " -sOutputFile=" + outfile + " " + infile)
