#!/usr/bin/env python
'''file: lee/lee.py
description: front end to J.-S. Lee's algorithm 2004
date: march 30, 2012. reimpl. 20170626
author: ashlin richardson'''
import os
import sys
from fl0w3r import args, run, error, normpath, read_config

if len(args) < 5:
    error("lee: jong-sen lee's algorithm igarss 2004 reimpl." +
          "20170626\n\nusage: lee [nwin] [nsmall] [nfinal] [niter]" +
          "\n\tnwin - the window size used in the freeman decomposition" +
          "\n\tnsmall - the number of seed classes (per category)" +
          "\n\tnfinal - the number of final classes (after merging)" +
          "\n\tniter - the number of wishart iterations")

nwin, nsmall = int(args[1]), int(args[2])
nfinal, niter = int(args[3]), int(args[4])

# read config file
n_row, n_col = read_config('config.txt')

# run freeman decomposition
cmd, nwin = "/home/" + os.popen("whoami").read().strip() + "/GitHub/polsarpro/Soft/bin/data_process_sngl/freeman_decomposition.exe", 5
myargs = [cmd,
          "-id ./",
          "-od ./",
          "-iodf T3",
          "-nwr " + str(nwin),
          "-nwc " + str(nwin),
          "-ofr 0",
          "-ofc 0",
          "-fnr " + str(n_row),
          "-fnc " + str(n_col)]
syscmd = (' ').join(myargs)
print('Running freeman 3-component decomposition from POLSARPRO...')
a = os.system(syscmd)

# run the lee algo
run('lee02.exe ./ ./ ' + str(nsmall) + ' ' + str(nfinal) + ' ' + str(niter))

'''
 Usage: 
freeman_decomposition.exe

Parameters:
 (string)	-id  	input directory
 (string)	-od  	output directory
 (string)	-iodf	input-output data format
 (int)   	-nwr 	Nwin Row
 (int)   	-nwc 	Nwin Col
 (int)   	-ofr 	Offset Row
 (int)   	-ofc 	Offset Col
 (int)   	-fnr 	Final Number of Row
 (int)   	-fnc 	Final Number of Col

Optional Parameters:
 (string)	-mask	mask file (valid pixels)
 (string)	-errf	memory error file
 (noarg) 	-help	displays this message
 (noarg) 	-data	displays the help concerning Data Format parameter

'''
