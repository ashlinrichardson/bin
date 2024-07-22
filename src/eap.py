#!/usr/bin/env python
import os
import sys
from misc import normpath, error, run
from envi import write_envi_hdr


def findfilestartstring(startstring, cwd):
    cmdtorun = str('find ' + cwd + ' -name ' + startstring + '*')
    _if = os.popen(cmdtorun).readlines()
    if(len(_if) > 1):
        print "Error: more than one VOL file found in directory "+cwd
        for lines in _if:
            print lines.strip()
        sys.exit(1)
    else:
        _if = (_if[0]).strip()
        _if = os.path.abspath(_if)
        return _if

if len(sys.argv) != 2:
    error("eas.py: Extract ALOS Palsar data to POLSARPRO/ENVI scattering " +
          "matrix format (binary) by Ash Richardson 20090716 " +
          " reimplemented 20170618." +
          "Usage: eas [Palsar data input directory (already unzipped)]")

cwd = sys.argv[1]
cwd = abspath(realpath(normcase(normpath(cwd))))
if(cwd[len(cwd) - 1] != '/'):
        cwd = cwd + '/'

'''
cmdtorun = str('find '+cwd+' -name "VOL*"')
_if = os.popen(cmdtorun).readlines()

if(len(_if) > 1):
    print "Error: more than one VOL file found in directory "+cwd
    for lines in _if:
        print lines.strip()
    sys.exit(1)
else:
    _if = (_if[0]).strip()


_if = os.path.abspath(_if)
'''

_if = findfilestartstring("VOL", cwd)

gdalinfo = os.popen('gdalinfo '+_if).readlines()
for g in gdalinfo:
    print g.strip()

for g in gdalinfo:
    if(len(g.split('Size is')) > 1):
        g = g.strip()
        g = g.split()
        NCOL = int(g[2].strip(','))
        NROW = int(g[3])

print 'NROW', str(NROW), 'NCOL', str(NCOL)

lines = ['Nrow', str(NROW), '---------', 'Ncol', str(NCOL), '---------',
         'PolarCase', 'bistatic', '---------', 'PolarType', 'full']

a = os.system("mkdir " + cwd + "/s2")

f = open(cwd + "config.txt", "wb")
for l in lines:
    f.write(l + "\n")
f.close()
f = open(cwd + "s2/config.txt", "wb")
for l in lines:
    f.write(l + "\n")
f.close()


hh = findfilestartstring("IMG-HH", cwd)
hv = findfilestartstring("IMG-HV", cwd)
vh = findfilestartstring("IMG-VH", cwd)
vv = findfilestartstring("IMG-VV", cwd)
os.system("mkdir " + cwd + "s2")
args = ['alos_convert_11_S2', str(hh), str(hv), str(vh), str(vv),
        str(cwd + "s2"), str(NCOL), str(0), str(0), str(NROW),
        str(NCOL), str(1), str(1), str(0), str(cwd + "config.txt")]
args = (" ").join(args)

''' print "/a.out ../eap/IMG-HH-ALPSRP094772890-P1.1__D
../eap/IMG-HV-ALPSRP094772890-P1.1__D
../eap/IMG-VH-ALPSRP094772890-P1.1__D
../eap/IMG-VV-ALPSRP094772890-P1.1__D .
 1248 0 0 18432 1248 1 1 0 ./config.txt"
'''
# print args
# a = os.system( args )
NLIN = NROW

# def write_envi_hdr(fn, nr, nc, nb):
a = write_envi_hdr(cwd + "s2/s11.bin", NLIN, NCOL, 1)
a = write_envi_hdr(cwd + "s2/s12.bin", NLIN, NCOL, 1)
a = write_envi_hdr(cwd + "s2/s21.bin", NLIN, NCOL, 1)
a = write_envi_hdr(cwd + "s2/s22.bin", NLIN, NCOL, 1)
print _if

# _if = sys.argv[1]
# extract the 2 bands (real and imaginary) to BIP floating point datatype file

# a=os.system("gdal_translate -b 1 -of ENVI "+_if+" "+cwd+"s11.bin")
# a=os.system("gdal_translate -b 2 -of ENVI "+_if+" "+cwd+"s12.bin")
# a=os.system("gdal_translate -b 3 -of ENVI "+_if+" "+cwd+"s21.bin")
# a=os.system("gdal_translate -b 4 -of ENVI "+_if+" "+cwd+"s22.bin")

a = os.system("gdal_translate -ot CFloat32 -b 1 -of PCIDSK " + _if + " " +
              cwd + "s2/s11.bin")
a = os.system("gdal_translate -ot CFloat32 -b 2 -of PCIDSK " + _if + " " +
              cwd + "s2/s12.bin")
a = os.system("gdal_translate -ot CFloat32 -b 3 -of PCIDSK " + _if + " " +
              cwd + "s2/s21.bin")
a = os.system("gdal_translate -ot CFloat32 -b 4 -of PCIDSK " + _if + " " +
              cwd + "s2/s22.bin")

# a=os.system("gdal_translate -ot CFloat32 -b 1 -of ENVI"+
# '''-co INTERLEAVE=BIP''' +_if+" "+cwd+"s11.bin")
# a=os.system("gdal_translate -ot CFloat32 -b 2 -of ENVI"+
# '''-co INTERLEAVE=BIP''' +_if+" "+cwd+"s12.bin")
# a=os.system("gdal_translate -ot CFloat32 -b 3 -of ENVI"+
# '''-co INTERLEAVE=BIP''' +_if+" "+cwd+"s21.bin")
# a=os.system("gdal_translate -ot CFloat32 -b 4 -of ENVI"+
# '''-co INTERLEAVE=BIP''' +_if+" "+cwd+"s22.bin")
# a=os.system("gdal_translate -ot CFloat32 -of GTiff "+_if+
# " "+cwd+"gtiff.tif")
# sys.exit(1)
#

'''

for fn in ['s11', 's12', 's21','s22']:
    fa = fn+".hdr"
    #a=os.system("mv "+cwd+fa+" "+cwd+fn+".hdr");
    f = open( cwd+fa,"r");
    fi = f.read();
    f.close();

#convert the ENVI header so that it represents a complex data type.
#The binary file is not altered, just the interpretation of ir by ENVI.
    out=((fi.replace("bands   = 2","bands   = 1")).replace(
      "data type = 4","data type = 6")).replace(
        "interleave = bip","interleave = bsq")
    out = out.replace("\nband names = {\nBand 1,\nBand 2}","")

    f = open(cwd+fa, "w");
    f.write( out);
    f.close();

#set the file name convention
    #a=os.system("mv "+cwd+fa+" "+cwd+fn+".bin.hdr");

'''
