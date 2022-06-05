#!/usr/bin/python
'''revised 20170826'''
import os
import sys
import time
import signal
import string
import asfcfg
import fnmatch
import subprocess
from string import *
from os.path import *

# Script Start
if (len(sys.argv) < 8) | (len(sys.argv) > 9):
    print ("prp.py: extract Palsar/Radarsat2, Faraday Correct, T4 Filter," +
           "Eigen Decomposition, HSV, Orthorectify." +
           " Adapted from 200909 program by J. Cobham")
    print ("Usage: prp [data input directory][output directory]" +
           "[filter type][filter size][alpha][value][hue]" +
           "[*dem(TIF)] *Optional")
    sys.exit(1)

ftype = sys.argv[3]
value = sys.argv[6]
hue = sys.argv[7]

if len(sys.argv) == 8:
    print "No dem specified..."
    print "Processing without orthorectification"
    dem = ""
else:
    dem = sys.argv[8]

i_dir = sys.argv[1]
i_dir = abspath(realpath(normcase(normpath(i_dir))))
if i_dir[len(i_dir) - 1] != '/':
    i_dir = i_dir + '/'

o_dir = sys.argv[2]
o_dir = abspath(realpath(normcase(normpath(o_dir))))
if (o_dir[len(o_dir) - 1] != '/'):
    o_dir = o_dir + '/'

t = (time.asctime()).split()
ta = [t[4], t[1], t[2], string.replace(t[3], ":", "")]
log = open(o_dir + "prp_logfile_" + string.join(ta, "_") + ".txt", "w")


log.write("COMMAND: scm\n")
log.write("TIMESTAMP:\n\t" + string.join(t, " ") + "\n")
log.write("COMMAND LINE ARGUMENTS\t" + string.join(sys.argv, " "))
log.write("\n")

# Palsar or Radarsat2 data
datatype = ""
workreport = ""  # Palsar only
metafile = ""  # Radarsat2 product.xml or Palsar LED-

l = os.listdir(i_dir)
for i in l:
    if i.startswith("ALPS"):
        workreport = i_dir + i
        log.write("Palsar workreport found in " + i_dir + "\n")
        print "\n*****Palsar Workreport Found!*****"
    elif i.startswith("LED"):
        datatype = "palsar"
        metafile = i_dir + i
        log.write("Palsar data found in " + i_dir + "\n")
        print "\n*****Palsar Data Found!*****"
    elif i.startswith("product.xml"):
        datatype = "radarsat2"
        metafile = i_dir + i
        log.write("Radarsat2 data found in " + i_dir + "\n")
        print "\n*****Radarsat2 Data Found!*****"

if datatype == "":
    log.write("No valid data found!\n")
    print "\n*****No valid data found!*****"
    sys.exit(1)

# Extract Palsar Data
if datatype == "palsar":
    log.write("Extracting Palsar Data\n")
    print "\nExtracting Palsar data"
    a = os.system("ep " + i_dir + " " + o_dir)

# Extract Radarsat2 Data
elif datatype == "radarsat2":
    print "\nExtracting RadarSAT2 data"
    log.write("Extracting Radarsat Data\n")
    os.system("ers " + i_dir + " " + o_dir)

# s2 data dir
s2_dir = o_dir + "s2/"

# Start scm3 with appropriate parameters
# Usage: scm3 [input directory] [output directory] [filter type] [filter size]
#  [Value histogram % cutoff (upper)] [faraday correction YES/NO]
#  [mapping function] [alpha 1/2/3] [Hue histogram %cutoff (lower)]
#  [vertical multilook factor]"
if datatype == "palsar":
    log.write("Running scm3 with palsar parameters\n")
    print "\nRunning scm3 with palsar parameters"
    a = os.system("scm3 " + s2_dir + " " + o_dir + " " + ftype + " " +
                  sys.argv[4] + " " + value + " " + "YES" + " " + "sch" +
                  " " + sys.argv[5] + " " + hue + " " + str(4))

if datatype == "radarsat2":
    log.write("Running scm3 with RadarSAT2 parameters\n")
    print "\nRunning scm3 with RadarSAT2 parameters"
    a = os.system("scm3 " + s2_dir + " " + o_dir + " " + ftype + " " +
                  sys.argv[4] + " " + value + " " + "NO" + " " + "sch" +
                  " " + sys.argv[5] + " " + hue + " " + str(1))

# generate Eigen Decompositions
log.write("Running cloudeT4\n")
a = os.system("cloudeT4 "+o_dir+"s2_fr_T4_"+ftype)

# Orthorectification
# Configure output file prefix and suffix
out_prefix = "o_"  # Set the output file prefix as you see fit...

prodate = " "  # Product date
if (datatype == "palsar") & (workreport != ""):
    # look for the Palsar product acquire date in the work report file
    f = open(workreport)
    lines = f.readlines()
    for l in lines:
        if l.startswith("Img_SceneStartDate"):
            l = l.split('"')  # actually we want to split at '"'
            l = l[len(l)-2].split(' ')
            prodate = l[0]
            f.close()
            break  # found the string so stop reading lines

if datatype == "radarsat2":
    # look for the Radarsat2 product acquire date in product.xml
    f = open(metafile)
    lines = f.readlines()
    for l in lines:
        l = l.strip()
        if l.startswith("<rawDataStartTime>"):
            l = l.strip('<rawDataStartTime>')
            l = l.split("T")
            prodate = l[0]
            f.close()
            break

if prodate != "":
    out_suffix = "_" + prodate
    # Set the output file suffix to the product date

else:
    out_suffix = "_"
    # Product date not found, use default output file suffix

# Find the files to ortho and create config files

if dem != "":
    print "\nStarting Orthorectification"
    log.write("\nStarting Orthorectification\n")
    asf_i_dir = o_dir + "s2_fr_T4_" + ftype
    imglist = []
    l = os.listdir(asf_i_dir)
    for i in l:
        if i.startswith("h_a_alpha"):
            m = os.listdir(asf_i_dir + "/" + i)
            for j in m:
                # Get a list of files for each image to orthorectify
                if fnmatch.fnmatch(j, '*.bin'):
                    imglist.append(asf_i_dir + "/" + i + "/" + j)
    if imglist == "":
        log.write("\nExiting. No Eigen Decompositions found!\n")
        print "\nExiting. No Eigen Decompositions found!"
        sys.exit(1)

    # Write the ASF config files and do the orthorectification
    asfcfg.writeASFConfig(imglist, datatype, metafile, dem, out_prefix,
                          out_suffix)

log.close()
print "\nCheck the logfile for errors"
print "\nCheck the orthorectification log files if applicable\n"
sys.exit(1)
