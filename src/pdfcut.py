#!/usr/bin/env python

import os,sys

args = sys.argv

if(len(args)<4):
	print("pdfcut.py.\nUsage: [firstpage# (from 1)] [lastpage #] [infile.pdf] [outfile.pdf]\n");
	sys.exit(1);

start = int(args[1])
end = int(args[2])
infile=args[3]
outfile=args[4]
if not os.path.exists(infile):
  print "Error: input file not found", infile
  sys.exit(1)

cmd = "gs -sDEVICE=pdfwrite -dNOPAUSE -dBATCH -dFirstPage="+str(start)+" -dLastPage="+str(end)+" -sOutputFile=" + outfile + " " + infile 
print cmd
a = os.system(cmd)

