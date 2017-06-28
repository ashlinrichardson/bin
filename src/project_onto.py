#!/usr/bin/env python
'''  projectOnto.py [A] [B] [C]

Project data from file A to match the coordinate system of file B

The output to be stored in file C
'''

import os
import sys
if len(sys.argv) < 4:
    print("projectOnto [src Dataset to project] " +
          "[target image to project onto] " +
          "[output file name]")
    sys.exit(1)

src_filename = os.path.abspath(sys.argv[1])
match_filename = os.path.abspath(sys.argv[2])
dst_filename = os.path.abspath(sys.argv[3])

if(src_filename == match_filename or
   src_filename == dst_filename or
   match_filename == dst_filename):
    print("Error: parameters must all be different.")
    sys.exit(1)

if(not(os.path.exists(src_filename)) or
   not(os.path.exists(match_filename))):
    print("Error: input parameters must exist.")
    sys.exit(1)

if(os.path.exists(dst_filename)):
    print("Error: destination filename already exists. Aborting.")
    exit(1)

try:
    from osgeo import gdal
    from osgeo import gdalconst
    pass
except:
    print("Error: gdal python API not available.")
    print("Please install with sudo easy_install GDAL.")
    sys.exit(1)

# Source
# src_filename = 'MENHMAgome01_8301/mllw.gtx'
src = gdal.Open(src_filename, gdalconst.GA_ReadOnly)
src_proj = src.GetProjection()
src_geotrans = src.GetGeoTransform()

# We want a section of source that matches this:
#  match_filename = 'F00574_MB_2m_MLLW_2of3.bag'
match_ds = gdal.Open(match_filename, gdalconst.GA_ReadOnly)
match_proj = match_ds.GetProjection()
match_geotrans = match_ds.GetGeoTransform()
wide = match_ds.RasterXSize
high = match_ds.RasterYSize

# Output / destination
# dst_filename = 'F00574_MB_2m_MLLW_2of3_mllw_offset.tif'
dst = gdal.GetDriverByName('GTiff').Create(dst_filename, wide, high, 1,
                                           gdalconst.GDT_Float32)
dst.SetGeoTransform(match_geotrans)
dst.SetProjection(match_proj)

# Do the work
gdal.ReprojectImage(src, dst, src_proj, match_proj, gdalconst.GRA_Bilinear)

del dst