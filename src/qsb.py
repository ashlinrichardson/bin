#!/usr/bin/python
'''do quick stats on a binary file, with respect to an image box
revised 20170826'''
import os
import sys
import time
import array
import string
from string import *
from math import sqrt
from os.path import *

if len(sys.argv) != 4:
    print("Usage: qsb [input binary file] [box file]" +
          " [path to dir. containing config.txt]")
    sys.exit(1)

binfile = sys.argv[1]
boxfile = sys.argv[2]
cwd = sys.argv[3]
# ['Tue', 'Apr', '28', '10:44:05', '2009']
# 0       1    2         3         4
# t = (time.asctime()).split()
# tf = t[4]+'_'+t[1]+'_'+t[2]+'_'+t[3]

binfile = abspath(binfile)

cwd = sys.argv[3]
cwd = abspath(realpath(normcase(normpath(cwd))))
if cwd[len(cwd) - 1] != '/':
    cwd = cwd + '/'

# logfile = open( cwd+tf+"_box_logfile.txt","w")
cmdstring = "qsb "
for i in range(1, len(sys.argv)):
    cmdstring = cmdstring + str((sys.argv)[i]) + " "
# logfile.write(cmdstring+"\n")

config_file = cwd + "config.txt"
f = open(config_file)
lines = f.readlines()
f.close()
NRow = int(lines[1].strip())
NCol = int(lines[4].strip())

# print "NRow",NRow, "NCol",NCol
f = open(boxfile)
lines = f.readlines()
l = ((lines[0].strip()).split())
nlines = 0
if len(l) != 1:
    print "Error: box file had incorrect format"
    exit(1)
else:
    nlines = int(l[0])

boxes = []
for i in range(1, nlines + 1):
    l = ((lines[i].strip()).split())
    if len(l) != 4:
        print ("Error: box file had incorrect format in line " +
               str(i + 1) + ", line read:")
        print lines[i]
        exit(1)
    boxes.append([int(l[0]), int(l[1]), int(l[2]), int(l[3])])
f.close()

data = array.array('f')
f = open(binfile, 'rb')
data.fromfile(f, NRow * NCol)
if len(data) == NRow * NCol:
    pass
else:
    print("Error: wrong number of bytes read.  Read" + str(len(data)) +
          " bytes, expected to read" + str(NRow * NCol) + " bytes.")
    sys.exit(1)

# print "xstart ystart xwidth ywidth min max
#        mean stdev #pixels filename boxnumber"
boxnum = 1
for b in boxes:
    pixelcount = (0)
    total = (0)
    square_total = (0)
    xstart = b[0]
    ystart = b[1]
    xend = xstart + (b[2])
    yend = ystart + (b[3])

    colstart = xstart
    colend = xend
    rowstart = ystart
    rowend = yend

    min = (data[((rowstart * NCol)+colstart)])
    max = (data[((rowstart * NCol)+colstart)])

    for col in range(colstart, colend):
        for row in range(rowstart, rowend):
            pixelcount += 1
            dat = (data[((row * NCol) + col)])
            total = total + dat
            if(dat < min):
                min = dat
            if(dat > max):
                max = dat
    if pixelcount != ((b[2]) * (b[3])):
        print "Error: wrong number of pixels was used in average\n"
        print "Got " + str(pixelcount) + " expected " + str((b[2]) * (b[3]))
        sys.exit(1)
    mean = total / pixelcount

    for col in range(colstart, colend):
        for row in range(rowstart, rowend):
            dat = data[((row * NCol) + col)]
            mdat = mean - dat
            square_total = square_total + (mdat * mdat)
    stdev = sqrt((square_total / (pixelcount)))
    print b[0], b[1], b[2], b[3], str(min), str(max), str(mean),\
          str(stdev), pixelcount, str(binfile), boxnum
    boxnum = boxnum + 1
