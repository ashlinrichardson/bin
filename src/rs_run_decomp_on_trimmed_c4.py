#!/usr/bin/env python
''' by Ash Richardson Jan. 5 2014 with edits 20170822
 - takes the directories $d$ in rs2folders.txt
 - calculates the least dimensions, of these folders
 - and trims the others to match the smallest.
 (*) the inputs are $d$/C4/
 (*) the output are $d$/C4/trim/'''

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
    f.write('\n'.join(lines))
    f.close()


def execute(ci, cmd):
    a = os.system(cmd)
    print "it", ci, "rt", a, cmd
    return(a)


ci = 0
for line in open("rs2folders.txt").readlines():
    l = line.strip()
    l = os.path.abspath(l)
    c4 = l + "/C4/C4_coreg/"
    cfg = c4 + "config.txt"
    c4trim = c4 + "trim/"
    cfg = c4trim + "config.txt"
    t3 = c4trim + "T3/"
    if not os.path.exists(cfg):
        print "Error: file not found: " + cfg
        sys.exit(1)
    else:
        nrow, ncol = rowCol(cfg)
        print nrow, ncol, cfg
        if not os.path.exists(t3):
            cmd = "mkdir " + os.path.abspath(t3)
            a = execute(ci, cmd)
            cmd = "cp " + cfg + " " + t3
            a = execute(ci, cmd)
        cmd = ("/Users/ash/flower/bin/correctC4 " + c4trim + " " +
               str(nrow) + " " + str(ncol) + " " + t3)
        print cmd
        a = execute(ci, cmd)
    ci += 1
    print "purging"
    a = os.system("purge")
