#!/usr/bin/env python
'''multilook a time-series set of radarsat2 data
unknown date with edits 20170825'''
import os
import sys
nlookRow = 4
nlookCol = 2
mlkCommand = "data_convert_MLK_S2.exe"


def rowCol(config):
    lines = open(config).readlines()
    nrow = int(lines[1].strip())
    ncol = int(lines[4].strip())
    return [nrow, ncol]


def execute(ci, cmd):
    a = os.system(cmd)
    print "it", ci, "rt", a, cmd


'''
data_convert_MLK_S2.exe.
TYPE: data_convert_MLK_S2 DirInput DirOutput Ncol OffsetLig
                          OffsetCol FinalNlig FinalNcol Symmetrisation
                          OutputDataFormat Nlook_col Nlook_lig
                          OutputDataFormatPP (if OutputDataFormat == IPP)'''

f = open("rs2folders.txt")
ci = 1
for l in f.readlines():
    fn = l.strip()
    s2dir = fn + "/s2/"
    config = s2dir + "config.txt"
    nrow, ncol = rowCol(config)
    print nrow, ncol, config
    c4dir = fn + "/C4/"
    execute(ci, "mkdir " + c4dir)
    execute(ci, mlkCommand + " " + s2dir + " " + c4dir + " " + str(ncol) +
            " 0 0 " + str(nrow) + " " + str(ncol) + " 0 C4 " + str(nlookCol) +
            " "+str(nlookRow))
    ci += 1
