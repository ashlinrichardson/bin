#!/usr/bin/python
'''create histogram for .bin (envi type 4 32-bit float binary data file)
user can select number of bins
unknown date-- revised 20170826'''
import os
import sys
from string import *
from os.path import *


def writeENVIheader(b, ncol, nlin, datatyp):
    datatyp = str(datatyp)
    fn = abspath(realpath(normcase(normpath(b)))) + ".hdr"
    f = open(fn, 'w')
    datatypstr = "data type = " + datatyp + "\n"
    dat = ["ENVI\n",
           "description = {\n",
           "Header written by enviheaders.py }\n",
           "samples = " + str(ncol) + "\n",
           "lines   = " + str(nlin) + "\n",
           "bands   = 1\n",
           "header offset = 0\n",
           "file type = ENVI Standard\n", datatypstr,
           "interleave = bsq\n",
           "sensor type = Unknown\n",
           "byte order = 0\n",
           "band names = {", str(fn) + "}\n"]
    for d in dat:
        f.write(d)
    f.close()
    print "w", fn
    return


def write_gri_1col(filename, title):  # figureletter, title):
    lines = [('open ' + str(filename)),
             '\nread columns x y',
             'set x name "Histogram Bin"',
             'set y name "'+str(title)+'"',
             'set x size 10',
             'set y size 10',
             '',
             'set line width 0.6 #1 #0.3              # points',
             'set line width axis 1 #0.4         # points',
             'set tic size 0.15               # centimetres',
             'draw curve\n\n',
             '',
             ' ',
             'quit']
    grifilename = filename + ".gri"
    f = open(grifilename, 'w')
    f.write(('\n').join(lines))
    f.write("\n")
    f.close()
    return grifilename


def write_gri_2col(filename, title):  # figureletter, title):
    lines = [('open ' + str(filename)),
             '\nread columns x y',
             'set x name "Histogram bin"',
             'set y name "' + str(title) + '"',
             'set x size 10',
             'set y size 10',
             '',
             'set line width 0.1              # points',
             'set tic size 0.15               # centimetres',
             'draw curve\n\n',
             ('open ' + str(filename)),
             'read columns x * y',
             'set x name "Histogram bin"',
             'set y name "' + str(title) + '"',
             'set x size 10',
             'set y size 10',
             '',
             'set line width 0.4              # points',
             'draw curve'
             ' ',
             'quit']
    grifilename = filename + ".gri"
    f = open(grifilename, 'w')
    f.write(('\n').join(lines))
    f.close()
    return grifilename


if len(sys.argv) != 4:
    print ("Usage: qspng" +
           "[binary input file] " +
           "[path to file config.txt]" +
           "[number of bins]")
    sys.exit(1)
argv = sys.argv
infile = abspath(sys.argv[1])
nbins = int(argv[3])
cwd = sys.argv[2]  # MAIN_DIRECTORY+ "/matlab_out/"
cwd = abspath(realpath(normcase(normpath(cwd))))

if cwd[len(cwd) - 1] != '/':
    cwd = cwd + '/'

config_file = cwd + "config.txt"

g = open(config_file)
gl = g.readlines()
g.close()
nlin = int(gl[1].strip())
ncol = int(gl[4].strip())

cmd = "qsh " + str(infile) + " " + str(cwd) + " " + str(nbins)
data = os.popen(cmd).readlines()

y = data[len(data) - 2]
x = data[len(data) - 1]

# print "y", y
# print "x", x

y = y.strip().split()

x = x.strip().split()

for i in range(0, len(y)):
    y[i] = float(y[i])
for i in range(0, len(x)):
    x[i] = float(x[i])

if len(y) != len(x):
    print "Error: list length mismatch"
    sys.exit(1)

txtfile = infile + ".txt"

f = open(txtfile, "wb")
for i in range(0, len(y)):
    f.write(str(x[i]) + " " + str(y[i]) + "\n")
f.close()

# print x
# print y


figtitle = str(infile)
figtitle = figtitle.strip().split("/")
figtitle = figtitle[len(figtitle) - 1]
grifile = write_gri_1col(txtfile, figtitle)

cmd = "gri " + grifile
# print cmd
a = os.system(cmd)

# print "OUTPUT file:"
print "w " + grifile + ".ps"
