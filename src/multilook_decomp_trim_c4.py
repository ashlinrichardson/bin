#!/usr/bin/env python
'''by Ash Richardson Jan. 5 2014 with mods 20170822
runs decompositions on trimmed C4 data

based on a script that:
takes the directories $d$ in rs2folders.txt
calculates the least dimensions, of these folders
and trims the others to match the smallest.
  the inputs are $d$/C4/
  the output are $d$/C4/trim/
'''

import os
import sys

FILTERSIZE = 2


def fileSize(fn):
    f = open(fn, "rb")
    # f is a file-like object.
    old_file_position = f.tell()
    f.seek(0, os.SEEK_END)
    size = f.tell()
    f.seek(old_file_position, os.SEEK_SET)
    return size


def rowCol(config):
    lines = open(config).readlines()
    nrow = int(lines[1].strip())
    ncol = int(lines[4].strip())
    return([nrow, ncol])


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
    return a

ci = 0
for line in open("rs2folders.txt").readlines():
    l = line.strip()
    l = os.path.abspath(l)
    c4 = l + "/C4/C4_coreg/"
    cfg = c4 + "config.txt"
    c4trim = c4 + "trim/"
    cfg = c4trim + "config.txt"
    t3 = c4trim + "T3/"
    # cfg = t3+"config.txt"
    if(not(os.path.exists(cfg))):
        print "Error: file not found: "+cfg
        sys.exit(1)
    else:
        nrow, ncol = rowCol(cfg)
        cfgNew = t3 + "config.txt"
        writeRowCol(cfgNew, nrow, ncol)
        print nrow, ncol, cfg
        cmd = "find " + os.path.abspath(t3) + ' -name "*.bin"'
        print cmd
        lines = os.popen(cmd).readlines()
        for li in lines:
            ll = li.strip()
            la = os.path.abspath(ll)
            if fileSize(la) == (4 * nrow * ncol):
                # valid binary file, for these dimensions.
                ofn = la + ".ml"
                cmd = "multilookRobust " + la + " " + t3 + " "
                cmd += ofn + " " + str(FILTERSIZE)
                # multilook [input file] [path to directory containing
                #    config.txt file] [output file] [window size]
                # print cmd
                a = execute(ci, cmd)
                if not a == 0:
                    print "Error: bad return code."
                    sys.exit(1)

'''
    if(not(os.path.exists(t3))):
      cmd = "mkdir "+os.path.abspath(t3)
      a = execute(ci, cmd)
      cmd = "/Users/ash/flower/bin/correctC4 "
      cmd += c4trim+" "+str(nrow)+" "+str(ncol)+" "+t3
    print cmd
    a = execute(ci, cmd)
  ci+=1
  print "purging"
  a = os.system("purge")

'''
