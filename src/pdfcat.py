#!/usr/bin/env python

import os,sys

args = sys.argv

if(len(args)<4):
	print("pdfcat.py.\nUsage: [file1] [file2] ... [filen] [outfilename.pdf]\n");
	sys.exit(1);

files = args[1:];
outfile = files[len(files)-1];

if os.path.exists(outfile):
  print "error: output file already exists"
  sys.exit(1)

infiles = files[:-1];
s = "";
for infile in infiles: 
	s = s+infile+" ";
print("input file names: "+s);#str(infiles));
print("output file name:  "+str(outfile));
# had -sPAPERSIZE=letter
cmd = "gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile="+outfile;
#   2030book.pdf cover2.pdf  2030.pdf
for infile in infiles: 
	cmd = cmd + " "+infile 	
print("\nexecuting command:");
print("    "+cmd)
a = os.system(cmd);

