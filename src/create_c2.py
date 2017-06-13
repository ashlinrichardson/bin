#!/usr/bin/env python
import os
import sys
nlookRow = 4
nlookCol = 2

mlkCommand = "data_convert_MLK_S2.exe"

def rowCol(config):
	lines = open(config).readlines()
	nrow = int(lines[1].strip())
	ncol = int(lines[4].strip())
  return([nrow, ncol])

def execute(ci, cmd):
	a = os.system(cmd)
	print "it", ci, "rt", a, cmd

'''
data_convert_MLK_S2.exe.
TYPE: data_convert_MLK_S2 DirInput DirOutput Ncol OffsetLig OffsetCol FinalNlig FinalNcol
Symmetrisation OutputDataFormat Nlook_col Nlook_lig OutputDataFormatPP (if OutputDataFormat == IPP)
'''

f = open("rs2folders.txt")
ci = 1;
cmd = 'matlab -nodisplay -r "'
for l in f.readlines():
	fn = l.strip()
  s2dir = fn + "/s2/"
  config = s2dir  +"config.txt"
	nrow, ncol = rowCol(config)
  print nrow, ncol, config
	c4dir = fn + "/C4/"
	#execute(ci,"mkdir "+c4dir)

cmd +=' exit;"'