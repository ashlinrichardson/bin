#!/usr/bin/env python
''' by Ash Richardson Jan. 5 2014 with mods 20170820
 takes the directories $d$ in rs2folders.txt
 calculates the least dimensions, of these folders
 and trims the others to match the smallest.
  the inputs are $d$/C4/
  the output are $d$/C4/trim/
'''
import os
import sys


def rowCol(config):
    lines = open(config).readlines()
    nrow = int(lines[1].strip())
    ncol = int(lines[4].strip())
    return [nrow, ncol]


def writeRowCol(config, row, col):
    lines = ["Nrow",
             "x",
             "---------",
             "Ncol",
             "x",
             "---------",
             "PolarCase",
             "bistatic",
             "---------",
             "PolarType",
             "full"]
    lines[1] = str(row)
    lines[4] = str(col)
    f = open(config, "wb")
    f.write("\n".join(lines))
    f.close()


def execute(ci, cmd):
    a = os.system(cmd)
    print "it", ci, "rt", a, cmd
    return(a)


cmd = "gcc -O2 trimBin.c -o trimBin;"
a = execute(0, cmd)
if a != 0:
    print("Error: failed to compile: trimBin.c")
    sys.exit(1)
else:
    print("Successfully compiled trimBin.c.")

# usage of c command line utility:
# trimBin [infile] [nrow] [ncol] [outfile] [nrow] [ncol]

ci = 0
minrow = 0
mincol = 0
for line in open("rs2folders.txt").readlines():
    l = line.strip()
    l = os.path.abspath(l)
    c4 = l + "/C4/C4_coreg/"
    cfg = c4 + "config.txt"
    if not os.path.exists(cfg):
        print "Error: file not found: " + cfg
        sys.exit(1)
    else:
        nrow, ncol = rowCol(cfg)
        print nrow, ncol, cfg
        if ci == 0:
            minrow = nrow
            mincol = ncol
        else:
            if nrow < minrow:
                minrow = nrow
            if ncol < mincol:
                mincol = ncol
    ci += 1

print "smallest dimensions:", minrow, mincol
print "Applying these to trim the C4 matrix files.."

files = ["C11.bin",
         "C13_real.bin",
         "C23_imag.bin",
         "C33.bin",
         "C12_imag.bin",
         "C14_imag.bin",
         "C23_real.bin",
         "C34_imag.bin",
         "C12_real.bin",
         "C14_real.bin",
         "C24_imag.bin",
         "C34_real.bin",
         "C13_imag.bin",
         "C22.bin",
         "C24_real.bin",
         "C44.bin"]

ci = 0
for line in open("rs2folders.txt").readlines():
    l = line.strip()
    l = os.path.abspath(l)
    c4 = l + "/C4/C4_coreg/"
    cfg = c4 + "config.txt"
    if not os.path.exists(cfg):
        print "Error: file not found: " + cfg
        sys.exit(1)
    else:
        di = 0
        nrow, ncol = rowCol(cfg)
        print nrow, ncol, cfg
        c4trim = c4 + "trim/"
        c4trim = os.path.abspath(c4trim) + "/"
        print c4trim
        cmd = "mkdir " + c4trim
        if not os.path.exists(c4trim):
            a = execute(ci, cmd)
        for f in files:
            cmd = "./trimBin " + c4 + f.strip() + " "
            cmd += str(nrow) + " " + str(ncol)
            cmd += " " + c4trim + f.strip() + " "
            cmd += str(minrow) + " " + str(mincol)
            a = execute(di, cmd)
            newcfg = c4trim + "config.txt"
            writeRowCol(newcfg, minrow, mincol)
            di += 1
        cmd = "eh " + c4trim
        a = execute(ci, cmd)

    ci += 1
