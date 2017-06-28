#!/usr/bin/env python2.7
'''stackRadarBinFiles.py A. Richardson 20160304; revised 20170628

    Stack *.bin files in a given directory into one (BSQ format) output bin
    file

    Check all the *.bin files in the input directory supplied have
      matching sizes. Abort if mismatch

    Otherwise "cat" all the *.bin files together into one *.bin file.
      An ENVI format header is written for the output file, with the names
      of the input files (minus the ".bin" extensions, of course) written
      as band names in the ENVI format header file'''

import os
import sys
argv = sys.argv

if len(argv) < 3:
    print argv, len(argv)
    print("stackRadarBinFiles.py [path to files] [output (stacked) filename]")
    sys.exit(1)


def abspath(x):
    return os.path.abspath(x)


def normcase(x):
    return os.path.normcase(x)


def normpath(x):
    return os.path.normpath(x)


def realpath(x):
    return os.path.realpath(x)


def file_size(x):
    return os.path.getsize(x)


def write_envi_hdr(fnBase, ncol, nlin, datatyp, band_names, n_bands):
    datatyp = str(datatyp)
    fn = abspath(realpath(normcase(normpath(fnBase)))) + ".hdr"
    f = open(fn, 'w')

    dat = ["ENVI\n", "samples = " + str(ncol) + "\n",
           "lines   = " + str(nlin) + "\n",
           "bands   = " + str(n_bands) + "\n",
           "header offset = 0\n",
           "file type = ENVI Standard\n",
           "data type = " + datatyp + "\n",
           "interleave = bsq\n",
           "sensor type = Unknown\n",
           "byte order = 0\n",
           "band names = {",
           str(band_names) + "}\n"]

    for d in dat:
        f.write(d)
    f.close()
    print "w", fn


def row_col(config_file):
    gl = open(config_file).readlines()
    nlin, ncol = int(gl[1].strip()), int(gl[4].strip())
    return nlin, ncol

bin_file_names = os.popen("ls -1 " + os.path.abspath(argv[1]) +
                          "/*.bin").readlines()
cmd = "cat"
f_s = i = 0
band_name_string = ""
n_bands = 0

for bf in bin_file_names:
    if i == 0:
        f_s = file_size(bf.strip())
    else:
        band_name_string += ","
        if(f_s != file_size(bf.strip())):
            print("Error: not all of the bin files were the same size.")
            sys.exit(1)
    ''' use file names without the extension, as band names.'''
    bfs = bf.strip().split("/")[-1]
    band_name_string += ((bfs)[:-4])
    i += 1
    cmd += (" " + bf.strip())
    n_bands += 1

''' combine the bin files together with "cat" '''
outFileName = os.path.abspath(argv[2])
cmd += (" > "+outFileName)
print cmd
a = os.system(cmd)

''' write the ENVI format header file '''
nrow, ncol = row_col(os.path.abspath(argv[1]) + "/config.txt")
write_envi_hdr(outFileName, ncol, nrow, 4, band_name_string, n_bands)
