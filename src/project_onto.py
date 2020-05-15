#!/usr/bin/env python
'''  projectOnto.py [A] [B] [C] [D]

Project data from file A to match the coordinate system of file B

The output to be stored in file C

Optional last parameter [D]: use nearest-neighbour resampling, not
bilinear which is the default'''

import os
import sys
if len(sys.argv) < 4:
    print("project_onto [src image to reproject] " +
          "[target img to project onto] " +
          "[output filename] [optional parameter: override bilinear and use nearest-neighbour]")
    sys.exit(1)

use_nearest = (len(sys.argv) >= 5)
if use_nearest:
    print("using nearest neighbour resampling..")

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
    print("Error: gdal python API not available.\n" + 
        "Please install with e.g., ./configure --with-python" +
        "Or, google: pip install gdal python")
    sys.exit(1)

# source image
src = gdal.Open(src_filename, gdalconst.GA_ReadOnly)
src_proj, src_geotrans = src.GetProjection(), src.GetGeoTransform()

# want section of source that matches:
match_ds = gdal.Open(match_filename, gdalconst.GA_ReadOnly)
match_proj, match_geotrans = match_ds.GetProjection(), match_ds.GetGeoTransform()
wide, high = match_ds.RasterXSize, match_ds.RasterYSize

# Output / destination # dst_filename = 'F00574_MB_2m_MLLW_2of3_mllw_offset.tif'
file_type = 'GTiff'
if dst_filename.split(".")[-1] == 'bin':
    default = 'ENVI'
print("driver_type", file_type)
dst = gdal.GetDriverByName(file_type)
dst = dst.Create(dst_filename, wide, high, 1, gdalconst.GDT_Float32)
dst.SetGeoTransform(match_geotrans)
dst.SetProjection(match_proj)

# Do the work
resamp = gdalconst.GRA_NearestNeighbour if use_nearest else gdalconst.GRA_Bilinear
gdal.ReprojectImage(src, dst, src_proj, match_proj, resamp)

del dst

'''
from https://gdal.org/python/osgeo.gdalconst-module.html, data types:

 	GDT_Byte = _gdalconst.GDT_Byte
  	GDT_UInt16 = _gdalconst.GDT_UInt16
  	GDT_Int16 = _gdalconst.GDT_Int16
  	GDT_UInt32 = _gdalconst.GDT_UInt32
  	GDT_Int32 = _gdalconst.GDT_Int32
  	GDT_Float32 = _gdalconst.GDT_Float32
  	GDT_Float64 = _gdalconst.GDT_Float64
  	GDT_CInt16 = _gdalconst.GDT_CInt16
  	GDT_CInt32 = _gdalconst.GDT_CInt32
  	GDT_CFloat32 = _gdalconst.GDT_CFloat32
  	GDT_CFloat64 = _gdalconst.GDT_CFloat64

sampling methods:
      	GRA_NearestNeighbour = _gdalconst.GRA_NearestNeighbour
  	GRA_Bilinear = _gdalconst.GRA_Bilinear
  	GRA_Cubic = _gdalconst.GRA_Cubic
  	GRA_CubicSpline = _gdalconst.GRA_CubicSpline
  	GRA_Lanczos = _gdalconst.GRA_Lanczos
  	GRA_Average = _gdalconst.GRA_Average
  	GRA_Mode = _gdalconst.GRA_Mode
  	GRA_Max = _gdalconst.GRA_Max
  	GRA_Min = _gdalconst.GRA_Min
  	GRA_Med = _gdalconst.GRA_Med
  	GRA_Q1 = _gdalconst.GRA_Q1
  	GRA_Q3 = _gdalconst.GRA_Q3
'''
