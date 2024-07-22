#!/usr/bin/env python
'''take H.bin, S.bin, V.bin files, histogram trim those.. reimpl. 20170625'''
import os
import sys
from misc import args, run, error

lo, hi = 0, 0

if len(args) < 2:
	  lo = hi = 2.
	  print "Attempting default settings..."
elif len(args) >= 3:
	  lo, hi = float(args[1]), float(args[2])
elif  len(args) >=2:
	  lo = hi = float(args[1])
else:
	  error("hsvtrim: by ash richardson March 16 2010+\n\tusage: hsvtrim " +
          "usage: hsvtrim [upper and lower percentage cut]\n\tusage: " + 
          "hsvtrim [lower cut] [upper cut]")
hl, hs = str(lo), str(hi)
run("htrim ./  H.bin " + hl + " " + hs + " Ht")
run("htrim ./  S.bin " + hl + " " + hs + " St")
run("htrim ./  V.bin " + hl + " " + hs + " Vt")
run("mv Ht H.bin")
run("mv St S.bin")
run("mv Vt V.bin")
