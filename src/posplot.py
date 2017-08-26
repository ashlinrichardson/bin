#!/usr/bin/python
'''unknown date. revised 20170825'''
import os
import sys

from string import *
from os.path import *


def write_gri_2col(filename, figureletter, title):
    lines = ['open ' + str(filename),
             '\nread columns x y',
             'set x name "Pixel index"',
             'set y name "'+str(title)+'"',
             'set x size 10',
             'set y size 10',
             '',
             'set line width 0.1              # points',
             'set tic size 0.15               # centimetres',
             'draw curve\n\n',
             ('open '+str(filename)),
             'read columns x * y',
             'set x name "Pixel index"',
             'set y name "'+str(title)+'"',
             'set x size 10',
             'set y size 10',
             '',
             'set line width 0.4              # points',
             'draw curve'
             ' ',
             'quit']

    f = open(filename + ".gri", 'w')
    f.write(('\n').join(lines))
    f.close()


def write_gri_1col(filename, figureletter, title):
    lines = [('open ' + str(filename)),
             '\nread columns x y',
             'set x name "Pixel index"',
             'set y name "'+str(title)+'"',
             'set x size 10',
             'set y size 10',
             '',
             'set line width 0.1              # points',
             'set line width axis 0.2         # points',
             'set tic size 0.15               # centimetres',
             'draw curve\n\n',
             '',
             ' ',
             'quit']
    f = open(filename + ".gri", 'w')
    f.write(('\n').join(lines))
    f.close()


def write_gri(filename):
    lines = [('open ' + str(filename)),
             'read columns x y z',
             'set x name "Pixel index"',
             'set y name "Value"',
             'set x size 10',
             'set y size 10',
             '',
             '#set x margin {rpn ..xsize.. ..xmargin.. + 1 +}',
             '#',
             '# Set line thickness for the curve to 1 point (0.3 mm) and the',
             '# axis line thickness to 0.2 points (0.1 mm).',
             'set line width 0.1              # points',
             'set line width axis 0.2         # points',
             '# Set the tics to be 1.5 mm.',
             'set tic size 0.15               # centimetres',
             '# Draw axes and frame, with axes offset from frame.  Some',
             '# people find this more attractive.',
             'set axes style offset',
             'draw axes 1',
             '# Now draw the actual curve.',
             'draw curve',
             '# Superimpose dots (radius 1.5 mm) at the data.',
             '#set symbol size 0.15',
             '#draw symbol bullet',
             '#',
             '# All done.',
             '# Draw a title above the plot.',
             'set font size 20',
             '\label = "Example 3 -- scales, axes, etc"',
             'draw label "\label" centered at \\',
             '    {rpn 2 5 + .5 + } \\',
             '    {rpn ..ytop.. yusertocm 2 +} cm ',
             ' ',
             ' ',
             'quit']
    f = open(filename+".gri", 'w')
    f.write(('\n').join(lines))
    f.close()

if len(sys.argv) < 3:
    print '(must execute from T3 data directory) create fig A-D, p.2160'
    print 'Lee, Schuler, Ainsworth, "Polarimetric SAR Data Compensation'
    print ' for Terrain Azimuth Slope Variation",IEEE TGARS VOL.38, NO.5,'
    print 'SEP. 2000\n\n'
    print 'Usage: posplot [line index] [# of pixels]'
    print 'Note: it is reccomended that you supply T3 data that has been'
    print 'filtered with a window size of 5, 7 or 9.'
    print 'Dependencies: this program depends on the command line utilities'
    print '"b2a", "p3", as well as the graphics library "gri"'
    sys.exit(1)

num_pix = int(sys.argv[2])
line_index = int(sys.argv[1])
if line_index < 0:
    print "Error: line index must be non-negative."
    sys.exit(1)

if num_pix <= 0:
    print "Error: number of pixels must be positive."
    sys.exit(1)


cwd = "./"
cwd = abspath(realpath(normcase(normpath(cwd))))
if (cwd[len(cwd) - 1] != '/'):
    cwd = cwd + '/'

config_file = cwd + "config.txt"
if not os.path.exists(config_file):
    print "Error: config.txt was not found."
    print "Program must be executed from within a T3 data directory"
    sys.exit(1)
else:
    print "Found the file config.txt."

f = open(config_file)
lines = f.readlines()
NRow = int(lines[1].strip())
NCol = int(lines[4].strip())
f.close()

if line_index >= NRow:
    print("Error: line index out of range.  This image has " +
          str(NRow) +
          " lines, so the index must be in the range (0," +
          str(NRow - 1) + ").")
    sys.exit(1)
if num_pix > NCol:
    print "Error: number of pixels must be no greater than the "
    print "number of pixels per line: " + str(NCol) + "."
    sys.exit(1)

# // open file.dat
# //   read columns x y
# //   draw curve
# ADD POS RUNNING LATER

T3_DIR = cwd
POS_DIR = cwd + "pos/"

print "NRow", NRow, "NCol", NCol
print "Attempting to create directory: ", POS_DIR
a = os.system("mkdir " + POS_DIR)

print "Attempting to run POS algorithm (p3)..."
a = os.system("p3 " + T3_DIR + " " + POS_DIR)

___FIGA = 0

B4_FIGB = 1
B4_FIGC = 2
B4_FIGD = 3
B4_FIGE = 4
B4_FIGF = 5

AF_FIGB = 6
AF_FIGC = 7
AF_FIGD = 8
AF_FIGE = 9
AF_FIGF = 10

ffn = []
ffn.append(POS_DIR + "theta")
fsl = ["B", "C", "D", "E", "F"]
fst = ["|HH-VV|^2", "|HV|^2", "|(HH-VV)HV*|",
       "|(HH+VV)HV*|", "|(HH+VV)(HH-VV)*|"]

fs = ["T22.bin", "FigC_HV_2.bin", "FigD_HHmVV_HV.bin",
      "FigE_HHpVV_HV.bin", "FigF_HHpVV_HHmVV.bin"]
for i in range(0, len(fs)):
    ffn.append(T3_DIR + fs[i])
for i in range(0, len(fs)):
    ffn.append(POS_DIR + fs[i])

FLOATS_TO_READ = num_pix
START_INDEX = (line_index) * NCol

for i in range(0, len(ffn)):
    print ffn[i]
    cmd_ = "b2a " + ffn[i] + " " + ffn[i] + ".txt "
    cmd_ += str(FLOATS_TO_READ) + " " + str(START_INDEX)
    print "\n" + cmd_
    a = os.system(cmd_)

for i in range(0, len(ffn)):
    print ffn[i] + ".txt"

for i in range(0, len(fs)):
    cmd_ = "joincol " + T3_DIR + fs[i] + ".txt "
    cmd_ += POS_DIR + fs[i] + ".txt " + POS_DIR + fs[i] + ".dat"
    print cmd_
    a = os.system(cmd_)
for i in range(0, len(fs)):
    write_gri_2col(POS_DIR + fs[i] + ".dat", fsl[i], fst[i])
    cmd_ = "gri " + POS_DIR + fs[i] + ".dat.gri"
    print cmd_
    a = os.system(cmd_)

write_gri_1col(POS_DIR + "theta.txt", "A", "Derived Orientation Angle")
cmd_ = "gri " + POS_DIR + "theta.txt.gri"

print cmd_
a = os.system(cmd_)


a = os.system("mv ./FigF_HHpVV_HHmVV.bin.dat.ps F.ps")
a = os.system("mv ./T22.bin.dat.ps              B.ps")
a = os.system("mv ./FigE_HHpVV_HV.bin.dat.ps    E.ps")
a = os.system("mv ./theta.txt.ps                A.ps")
a = os.system("mv ./FigD_HHmVV_HV.bin.dat.ps    D.ps")
a = os.system("mv ./FigC_HV_2.bin.dat.ps        C.ps")

if(os.path.exists("./posplot.ps")):
    a = os.system("mv ./posplot.ps posplot.ps.bak")

a = os.system("gs -q -dNOPAUSE -dBATCH -sDEVICE=pswrite " +
              "-sOutputFile=posplot.ps A.ps B.ps C.ps D.ps E.ps F.ps")
if(os.path.exists("./posplot.ps")):
    print "\n\nPolarization Orientation Correction plots"
    print "\tshould have been successfully generated."
    print "(Output file was found)."
    print "Plots A-F are stored in ./posplot.ps"
    print "\n\nDone."
else:
    print "Error: output file not found.  Process failed."
    print "Sorry that this error message not very descriptive"

if(os.path.exists("./eta")):
    os.system("rm -rf ./eta")

# fig A = theta
# diff figs:
# fig B = T22
