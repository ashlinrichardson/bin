'''20221105 burn_nan.py

take nan from one file, and write it in the same position to each (other) .bin file in the target folder 

'''
import os
import sys
from envi import read_envi_hdr
from misc import error, normpath, run
abspath = os.path.abspath
sep = os.path.sep
args = sys.argv
err = error

if len(args) < 3:
    err("burn_nan.py [.bin file to burn nan from] [folder with .bin files to burn nan into]")

n = abspath(args[1])
d = abspath(args[2])

if not os.path.exists(n) or not os.path.exists(d) or not os.path.isdir(d):
    err("please check input files")

if n[-3:] != 'bin': 
    err(".bin file required as first paramter")
   
bins = [x.strip() for x in os.popen("ls -1 " + d + sep + "*.bin").readlines()]
for b in bins:
    if b == n:
        continue
    X = read_envi_hdr(b[:-3] + 'hdr')  # lines, samples, bands
    run(' '.join(['raster_burn_zeros', n, b]))
