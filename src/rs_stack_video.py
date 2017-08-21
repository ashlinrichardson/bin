#!/usr/bin/env python
import os
import sys
print("Did you run processAll.m(2, masterI)?")


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


def rowCol(hdrfn):
    # read ENVI header with header file name hdrfn
    lines = open(hdrfn).readlines()
    nrow = 0
    ncol = 0
    for line in lines:
        if matchField(line, "samples"):
            ncol = extractField(line, "samples")
        if matchField(line, "lines"):
            nrow = extractField(line, "lines")
    return [nrow, ncol]

lines = os.popen('find ./ -name "C2_*"').readlines()
stackDirs = []
for l in lines:
    line = l.strip()
    if len(line.split("_")) == 2:
        sl = line.split("_")
        try:
            int(sl[1])
            stackDirs.append(os.path.abspath(line))
        except:
            pass

nTracks = len(stackDirs)

dirs = []
minrow = 0
mincol = 0
mini = 1
for i in range(1, nTracks + 1):
    s = os.path.abspath("C2_"+str(i)) + "/"
    dirs.append(s)
    hfn = s+"/C11.bin.hdr"
    if not os.path.exists(hfn):
        print("Error: header file not found.")
        sys.exit(1)
    nrow, ncol = rowCol(hfn)
    print nrow, ncol
    if i == 1:
        minrow = nrow
        mincol = ncol
        mini = 1
    else:
        if nrow < minrow:
            minrow = nrow
            mini = i
        if(ncol < mincol):
            mincol = ncol
            mini = i

print "\n\nsmallest dimensions are:"
print minrow, mincol
print "For dataset: ", str(mini), " of 1 to", str(nTracks)

if os.system("gcc trimBin.c -o trimBin") != 0:
    print("Error: trimBin.c failed to compile.")
    sys.exit(1)
if os.system("g++ abs2.cpp -o abs2") != 0:
    print("Error: abs2.cpp failed to compile.")
    sys.exit(1)


files = ["C11.bin", "C12_real.bin", "C12_imag.bin", "C22.bin"]

for d in dirs:
    trimdir = os.path.abspath(d) + slash() + "trim"
    if not os.path.exists(trimdir):
        os.makedirs(trimdir)
    row, col = rowCol(d + "/C11.bin.hdr")
    nrow = minrow
    ncol = mincol

    for f in files:
        ff = d + slash() + f
        assertFile(ff)
        fo = trimdir + slash() + f
        hfn = fo + ".hdr"
        cmd = "./trimBin " + ff + " "
        cmd += str(row) + " " + str(col)
        cmd += " " + fo + " " + nrow + " " + ncol
        writeENVIHeader(hfn, nrow, ncol, 1)
        a = os.system(cmd)
        # abs [NROW] [NCOL] [real binary file ENVI type 4]
        # [imag binary file ENVI type 4] [outfile]
    realFILE = trimdir + slash() + "C12_real.bin"
    imagFILE = trimdir + slash() + "C12_imag.bin"
    oFILE = trimdir + slash() + "C12_abs.bin"
    cmd = "./abs2 " + str(nrow) + " " + str(ncol) + " "
    cmd += realFILE + " " + imagFILE + " " + oFILE
    a = os.system(cmd)
    print a, cmd
    C11file = trimdir + slash() + "C11.bin"
    C12file = oFILE
    C22file = trimdir + slash() + "C22.bin"
    file3b = trimdir + slash() + "rgb.bin"
    cmd = "cat " + C11file + " " + C12file + " " + C22file + " > " + file3b
    a = os.system(cmd)
    writeENVIHeader(file3b + ".hdr", nrow, ncol, 3)
