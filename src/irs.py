#!/usr/bin/env python
import re
import os
import sys
from ansicolor import KRED, KGRN, KNRM, KMAG
from fl0w3r import chkdir, error, normpath, config_file, wopen, printw

args = sys.argv
if len(args) < 2:
    msg = 'irs.py: import Radarsat-2 header,'
    msg += 'reimplemented 20170605 from 200808 version'
    error(msg + 'Usage: irs [Radarsat-2 data directory]')

chkdir(args[1])
cwd = normpath(args[1])

cfg_fn, xml_fn = cwd + "config.txt", cwd + "product.xml"

L = open(xml_fn).read()

if len(re.findall(r'<satellite>RADARSAT-2</satellite>', L)) != 1:
    error('not known RADARSAT-2 product format')

rf = re.findall

def cut(s):
    # from, e.g., 'lineScale>3932</lineScale' get 3932
    return s[0].split('<')[0].split('>')[1]

def get(expr):
    global L
    val = eval("cut(rf(r'" + expr + ".*" + expr + "', L))")
    # print KGRN + expr + KMAG + ' = ' + KRED + val
    return val

product_id = get('productId')
acquisition_type = get('acquisitionType')
polarizations =  get('polarizations')
product_type =  get('productType')

line_scale = get('lineScale')
pixel_scale = get('pixelScale')
latitude_scale = get('latitudeScale')
longitudeScale = get('longitudeScale')

J = L.split("rasterAttributes")
L = J[1]

samples_per_line = get('numberOfSamplesPerLine')
number_of_lines = get('numberOfLines')
sampled_pixel_spacing = get('sampledPixelSpacing')
sampled_line_spacing = get('sampledLineSpacing')

if polarizations != "HH VV HV VH":
    error('polarizations HH VV HV VH not found')

wopen(cfg_fn).write(config_file(number_of_lines, samples_per_line))
printw(cfg_fn)

print KMAG + "Recommended vertical multilook factor:" + KGRN
print '\t' + KRED, int(round( float(line_scale) / float(pixel_scale))), KNRM



