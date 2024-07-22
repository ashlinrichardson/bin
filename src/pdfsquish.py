#!/usr/bin/env python
import os
import sys
from misc import args, normpath, run, error

if len(args) < 3:
    error("pdfsquish.py.\nUsage: [infile.pdf] [outfile.pdf]" +
          "\nReduce pdf file size to screen resolution")

infile, outfile = normpath(args[1]), normpath(args[2])

if not os.path.exists(infile):
    error("input file not found" + str(infile))

run("gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.4 -dPDFSETTINGS=/ebook " + # screen
    "-dNOPAUSE -dQUIET -dBATCH -sOutputFile=" + outfile + " " + infile)
