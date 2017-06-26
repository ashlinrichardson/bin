#!/usr/bin/env pythyon
'''20170626 reimpl'''
import os
import sys
from math import floor
from fl0w3r import args, error, run, normpath, read_config

if len(args) < 4:
    error("mla1p.py: multilook all binary files in dir, 1% scaling." +
          "Usage: mla [input path] [output path] [window size]")

inp, out = normpath(args[1]), normpath(args[2])
win = int(args[3])

config_file = inp + "/config.txt"

nlin, ncol = read_config(config_file)

result = os.popen("find "+inp+" -name \"*.bin\"").read()
'''Usage: multilook [input file] [path to directory containing config.txt file]
[output file] [window size]'''

result = result.strip().split()
run('cp ' + inp + 'config.txt ' + out + 'config.txt')

for infile in result:
    fn = normpath(infile.strip())
    run("multilook " + infile + " " + inp + " " + out + "/" + fn + " " +
        str(win))
    run("htrim " + out + "/  " + out + "/" + fn + " 1 1 " + out + "/" +
        fn + ".ht")
