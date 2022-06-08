#!/usr/bin/env python
'''file: lee/lee.py
description: front end to J.-S. Lee's algorithm 2004
date: march 30, 2012. reimpl. 20170626
author: ashlin richardson'''
import os
import sys
from fl0w3r import args, run, error, normpath

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
cmd, nwin = "freeman_decomposition_T3.exe", 5
myargs = [cmd, "./", "./", str(nwin), str(0), str(0), str(n_row), str(n_col)]
syscmd = (' ').join(myargs)
print('Running freeman 3-component decomposition from POLSARPRO...')
run(syscmd)

# run the lee algo
run('lee02.exe ./ ./ ' + str(nsmall) + ' ' + str(nfinal) + ' ' + str(niter))
