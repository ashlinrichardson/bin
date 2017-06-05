#!/usr/bin/env python
import re
import os
import sys 
#from string import *
from ansicolor import KRED, KGRN, KNRM, KMAG
from fl0w3r import chkdir, error, normpath, config_file, wopen

args = sys.argv
if len(args) < 2:
  msg = 'irs.py: import Radarsat-2 header, reimplemented 20170605 from 200808 version'
  error(msg + 'Usage: irs [Radarsat-2 data directory]')

chkdir(args[1])
cwd = normpath(args[1])

cfg_fn, xml_fn = cwd + "config.txt", cwd + "product.xml"

L = open(xml_fn).read()

if len(re.findall(r'<satellite>RADARSAT-2</satellite>', L)) != 1:
    error('not known RADARSAT-2 product format')

product_id = (((((re.findall(r'productId.*productId', L))[0]).split('<'))[0]).split('>'))[1]
acquisition_type = (((((re.findall(r'acquisitionType.*acquisitionType', L))[0]).split('<'))[0]).split('>'))[1]
polarizations = (((((re.findall(r'polarizations.*polarizations', L))[0]).split('<'))[0]).split('>'))[1]
product_type = (((((re.findall(r'productType.*productType', L))[0]).split('<'))[0]).split('>'))[1]
			
m = KMAG + ','
print KGRN + "productId" + m + KRED, product_id
print KGRN + "acquisitionType" + m + KRED, acquisition_type
print KGRN + "polarizations" + m + KRED, polarizations
print KGRN + "productType" + m + KRED, product_type
J = L.split("rasterAttributes")
L = J[1]

number_of_samples_per_line = (((((re.findall(r'numberOfSamplesPerLine.*numberOfSamplesPerLine', L))[0]).split('<'))[0]).split('>'))[1]
number_of_lines = (((((re.findall(r'numberOfLines.*numberOfLines', L))[0]).split('<'))[0]).split('>'))[1]
sampled_pixel_spacing = (((((re.findall(r'sampledPixelSpacing.*sampledPixelSpacing', L))[0]).split('<'))[0]).split('>'))[1]
sampled_line_spacing = (((((re.findall(r'sampledLineSpacing.*sampledLineSpacing', L))[0]).split('<'))[0]).split('>'))[1]

print KGRN + "numberOfSamplesPerLine" + m + KRED, number_of_samples_per_line
print KGRN + "numberOfLines" + m + KRED, number_of_lines
print KGRN + "sampledPixelSpacing" + m + KRED, sampled_pixel_spacing
print KGRN + "sampledLineSpacing" + m + KRED, sampled_line_spacing

if polarizations != "HH VV HV VH":
    error('polarizations HH VV HV VH not found')

wopen(cfg_fn).write(config_file(number_of_lines, number_of_samples_per_line))

a, b = float(sampled_pixel_spacing), float(sampled_line_spacing)

fact, last_dif, lasti, lastj, sfact = a / b, 100., 1, 1, 9
for i in range(1, sfact):
    fi = float(i)
    for j in range(1, sfact):
        nfact = fi / float(j)
        if(abs(nfact - fact) < last_dif):
            last_dif= abs(nfact - fact)
            lasti, lastj = i, j
print KMAG + "\nrecommended Multilook Factors (of " + str(sfact) + " or less):"
print KGRN + "\tCol: " + KRED, lasti
print KGRN + "\tRow: " + KRED, lastj

if (lasti == 1) & (lastj == 1):
    print KRED + '\nConclusion: ' + KGRN + 'Pixels are reasonably square' + KNRM
