#!/usr/bin/env python
# by Ash Richardson January 6, 2014 with edits 20170822
# takes the directories $d$ in rs2folders.txt
# calculates time averages of multilooked parameters {$d$/C4/trim/T3/*.bin.ml}

import os
import sys


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


def writeENVIHeader(hfn, nrow, ncol, nb):
    print hfn
    f = open(hfn, "wb")
    f.write("ENVI\n")
    f.write("samples = " + str(ncol) + "\n")
    f.write("lines   = " + str(nrow) + "\n")
    f.write("bands   = " + str(nb) + "\n")
    f.write("header offset = 0\n")
    f.write("file type = ENVI Standard\n")
    f.write("data type = 4\n")
    f.write("interleave = bsq\n")
    f.write("sensor type = Unknown\n")
    f.write("byte order = 0")
    f.close()


def slash():
    return "/"


def assertFile(f):
    if not os.path.exists(f):
        print("Error: file " + str(f) + " does not exist")
        exit(1)
    return


def matchField(line, field):
    if line[:len(field)] == field:
        return True
    else:
        return False


def extractField(line, field):
    if not matchField(line, field):
        print("Error: field did not match line")
        sys.exit(1)
    else:
        sl = line.strip().split("=")
        if len(sl) != 2:
            print("Error: not defined for lines with multiple equals signs.")
            sys.exit(1)
        return sl[1].strip()


def rowColENVI(hdrfn):
    # read ENVI header with header file name hdrfn
    lines = open(hdrfn).readlines()
    nrow = 0
    ncol = 0
    for line in lines:
        if matchField(line, "samples"):
            ncol = extractField(line, "samples")
        if matchField(line, "lines"):
            nrow = extractField(line, "lines")
    return [int(nrow), int(ncol)]

ci = 0
dataFolders = []
myFiles = {}
for line in open("rs2folders.txt").readlines():
    l = line.strip()
    l = os.path.abspath(l)
    c4 = l + "/C4/C4_coreg/"
    cfg = c4 + "config.txt"
    c4trim = c4 + "trim/"
    cfg = c4trim + "config.txt"
    t3 = c4trim + "T3/"
    cfg = t3 + "config.txt"
    dataFolders.append(t3)
    myFiles[t3] = []
    if not os.path.exists(cfg):
        print "Error: file not found: " + cfg
        sys.exit(1)
    else:
        nrow, ncol = rowCol(cfg)
        print nrow, ncol, cfg
        cmd = "find " + os.path.abspath(t3) + ' -name "*.bin.ml"'
        print cmd
        lines = os.popen(cmd).readlines()
        for li in lines:
            ll = li.strip()
            la = os.path.abspath(ll)
            assertFile(la)
            nrow, ncol = rowColENVI(la + ".hdr")
            if fileSize(la) == (4 * nrow * ncol):
                # valid binary file, for these dimensions.
                print fileSize(la), la
                myFiles[t3].append(la)
                # ofn = la+".ml"
                # cmd = "multilookRobust "+la+" "+t3+" "+ofn+" " +
                #     str(FILTERSIZE);
                # multilook [input file]
                #    [path to directory containing config.txt file]
                #    [output file] [window size]
                # print cmd
                # a= execute(ci,cmd)
                # if(not(a==0)):
                # print "Error: bad return code."
                # sys.exit(1)

ci = 0
dn = 0
for d in dataFolders:
    print d, len(myFiles[d])
    if ci == 0:
        dn = len(myFiles[d])
    else:
        if len(myFiles[d]) != dn:
            print("Error: number of .bin.ml files did not match.")
            sys.exit(1)
    ci += 1

ci = 0
outdir = "T3TF"
outdir = os.path.abspath(outdir) + slash()
fns = ["min", "max", "med", "mad", "avg", "std"]

for d in dataFolders:
    if ci == 0:
        print d, len(myFiles[d])
        print("\n\n")
        for i in range(0, len(myFiles[d])):
            fn = (myFiles[d])[i]
            frags = fn.strip().split(slash())
            fbase = frags[len(frags) - 1]
            print fbase
            ofbase = outdir + fbase
            cmd = "/Users/ash/flower/bin/tsa "
            nfiles = len(dataFolders)
            cmd = cmd + str(nfiles)
            nrow = 0
            ncol = 0
            for j in range(0, len(dataFolders)):
                dfn = dataFolders[j]
                fn = dfn + fbase
                print "\t" + fn
                nrow, ncol = rowColENVI(fn.strip() + ".hdr")
                if j == 0:
                    cmd = cmd + " " + str(nrow) + " " + str(ncol)
                cmd = cmd + " " + fn.strip()

            for fnb in fns:
                ofns = ofbase + "." + fnb.strip()
                print "ofns", ofns
                cmd = cmd + " " + ofns.strip()
            print "\t\t" + cmd
            a = execute(ci, cmd)
            print a
    ci += 1
