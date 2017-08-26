#!/usr/bin/python
'''write histograms to postscript (.ps) format,
for all ENVI type-4 binary files in folder
date unknown-- revised 20170826'''
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
           "file type = ENVI Standard\n",
           datatypstr,
           "interleave = bsq\n",
           "sensor type = Unknown\n",
           "byte order = 0\n",
           "band names = {", str(fn) + "}\n"]
    for d in dat:
        f.write(d)

    f.close()
    print "w", fn
    return


if len(sys.argv) != 3:
    print "Usage: qspsa [input directory] [NBINS]"
    print "Print .ps format histograms for all binary files"
    print "\tin directory (type 4 ENVI)."
    sys.exit(1)

cwd = sys.argv[1]  # MAIN_DIRECTORY+ "/matlab_out/"
cwd = abspath(realpath(normcase(normpath(cwd))))

NBINS = int(sys.argv[2])

if cwd[len(cwd) - 1] != '/':
    cwd = cwd + '/'

config_file = cwd + "config.txt"

g = open(config_file)
gl = g.readlines()
g.close()
nlin = int(gl[1].strip())
ncol = int(gl[4].strip())

# os.system("ls "+cwd+"*.bin > ls1234.txt")
# f=open("ls1234.txt")
# bins=f.read()
# f.close()

bins = os.popen("ls "+cwd+"*.bin").read()
bins = bins.strip()

bins = bins.replace("\n", " ")
bins = bins.strip().split(" ")
a = bins[0].split(".")

# print bins[0]
if a[(len(a) - 1)] == "bin":
    pass
else:
    print "no bin files in directory"
    sys.exit(1)


PSFILES = []

# os.system("rm -f ls1234.txt")
for b in bins:
    b = abspath(realpath(normpath(b)))
    splitdir = b.strip().split("/")
    fn = splitdir[len(splitdir)-1]
    datatype = 4
    if(fn[0] == 'T'):
        datatype = 4
    elif((fn[0] == 's') & ((fn[1] == '1') | (fn[1] == '2'))):
        datatype = 6
        # complex scattering matrix data (polsarpro format).
    elif(((fn[0] == 'R') | (fn[0] == 'L')) &
         ((fn[1] == 'R') | (fn[1] == 'L'))):
        datatype = 6  # circular polarization complex data written by circ.cpp.
    else:
        datatype = 4

    cmd = "qsps " + b + " " + cwd + " " + str(NBINS)
    print cmd
    a = os.system(cmd)
    PSFILES.append(b + ".txt.ps")

cmd = "gs -q -dNOPAUSE -dBATCH -sDEVICE=pswrite -sOutputFile="
cmd += cwd + "/qspsa.ps "

for ps in PSFILES:
    cmd = cmd + ps.strip() + " "

os.system(cmd)

print cmd
print "\nOUTPUT FILE:"
print os.path.normpath(cwd + "/qspsa.ps")

'''
psbakdir = +cwd+"/psbak"
a=os.system("mkdir "+psbakdir)
a=os.system("mv -v "+cwd+"/*.ps "+psbakdir)
a=os.system(cmd)
'''
