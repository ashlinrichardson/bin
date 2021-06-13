'''using gdalinfo and snap (gpt), add an envi-type-13 index band to a product

tested with input product being file path, within which we can find manifest.safe'''

import os
import sys
args = sys.argv
sep = os.path.sep

p = None
if len(args) < 2:
    p = "."
p = os.path.abspath(p) + sep
lines = os.popen("gdalinfo " + p + "imagery/*HH*tif").readlines()

nrow, ncol = 0, 0
for i in range(0, len(lines)):
    w = lines[i].strip().split()
    try:
        if w[0] == 'Size' and w[1] == 'is':
            print(w)
            ncol = int(w[2].strip(','))
            nrow = int(w[3])
            break
    except:
        pass


c = 'ind_image ' + str(nrow) + " " + str(ncol) + " " + p + "ix.bin " + p + "ix.hdr"
a = os.system(c)

'''
Usage:
  gpt BandMerge [options] 

Description:
  Allows copying raster data from any number of source products to a specified 'master' product.


Parameter Options:
  -PgeographicError=<float>                   Defines the maximum lat/lon error in degree between the products.
                                              Default value is '1.0E-5f'.
  -PsourceBands=<string,string,string,...>    The list of source bands.

------------------------
Usage:
  gpt Merge [options] 

Description:
  Allows merging of several source products by using specified 'master' as reference product.


Source Options:
  -SmasterProduct=<file>    The master, which serves as the reference, e.g. providing the geo-information.
                            This is a mandatory source.

Parameter Options:
  -PgeographicError=<float>    Defines the maximum lat/lon error in degree between the products. If set to NaN no check for compatible geographic boundary is performed
                               Default value is '1.0E-5f'.

'''

gpt = '/home/' + os.popen('whoami').read().strip() + '/snap/bin/gpt'
c = (gpt + ' Merge -SmasterProduct=' + p + 'manifest.safe' + ' ' + '-SsourceProducts=' + p + 'ix.hdr' + 
              ' -t "' + p + 'merged.dim"')

print(c)
