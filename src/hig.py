#!/usr/bin/env python
'''visualize multi-band image as interlaced gif 20170625'''
import os
import sys
import math
import array
from envi import read_envi_hdr
import matplotlib.image as mpimg
from fl0w3r import run, normpath, exists, error, args, chkfile, wopen


def bad(x):
    return math.isinf(x) or math.isnan(x)

if len(args) < 2:
    error("hig.py: make interlaced gif from hyperspectral images." +
          "By Ash Richardson, May 5, 2009." +
          "\tUsage: hi [ENVI format hyperspectral image (data type 4)]")

    
fn = normpath(args[1])
hfn = fn + ".hdr"
chkfile(fn)
chkfile(hfn)


# read image dimensions from header file
lines, samples, bands = read_envi_hdr(hfn)
lines, samples, bands = int(lines), int(samples), int(bands)


data = array.array('f')
data.fromfile(open(fn), samples * lines * bands)
npx = samples * lines

for b in range(0, bands):
    band = data[b * npx: (b + 1) * npx]
    mn, mx = band[0], band[0]
    if bad(band[0]):
        error("first data entry was bad")

    for i in range(0, len(band)):
        di = band[i]
        if not bad(di):
            mn = di if di < mn else mn
            mx = di if di > mx else mx
    print "band", b, "min", mn, "max", mx

    for i in range(0, len(band)):
        band[i] = 255. * (band[i] - mn) / (mx - mn)
        band[i] = band[i] if not bad(band[i]) else 0.
    
    fn = 'jpg' + str(b).zfill(5) + '.jpg'
    dat, ci = [], 0
    for i in range(0, lines):
        line = []
        for j in range(0, samples):
            d = band[ci]
            d = 0. if math.isnan(d) else d
            line.append(d)
            ci += 1
        dat.append(line)
    mpimg.imsave(fn, dat)

run('convert jpg00*.jpg hig.gif')
