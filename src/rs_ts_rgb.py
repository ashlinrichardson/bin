#!/usr/bin/env python
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
    f.write("\n".join(lines))
    f.close()


def execute(ci, cmd):
    a = os.system(cmd)
    print "it", ci, "rt", a, cmd
    return a


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
    if not(os.path.exists(f)):
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

lines = os.popen('find ./ -name "*.ml.min"').readlines()
end = ['min', 'max', 'avg']

for l in lines:
    line = l.strip()
    line = line.strip(".")
    line = line.strip("/")
    row, col = rowColENVI(line + ".hdr")
    print row, col
    line = line[:-3]
    cmd = "cat "
    for e in end:
        cmd += (" " + line + e)
    rgbfn = line + 'rgb'
    cmd += (" > " + rgbfn)
    rgbhfn = rgbfn + ".hdr"
    writeENVIHeader(rgbhfn, row, col, 3)
    print cmd
    a = os.system(cmd)
