#!/usr/bin/env python
'''description: write envi headers for type-4 envi files in a directory with
config.txt present. date: march 30, 2012 reimplemented 20170621 arichardson'''

import os
import sys
from envi import write_envi_hdr
from fl0w3r import run, normpath, error, args, read_config, exists

if len(args) < 2:
    error('Usage: eh [input directory]')

cwd = normpath(args[1])

# read image dimensions from config file
config_file = cwd + 'config.txt'
nlin, ncol = read_config(config_file)

# list a files in cwd with filenames: *.bin 
bins = os.popen('ls -1 ' + cwd + '*.bin').read().strip().split('\n')

# quit if no bin file
if bins[0].strip().split('.')[-1] != 'bin':
    error('no *.bin files in directory: ' + cwd)

for b in bins:
  hfn, b = None, normpath(b.strip())
  fn = '.'.join(b.split('.')[:-1])  # filename without extension
  if exists(b + '.hdr'):
      hfn = b + '.hdr'
  elif exists(fn + '.hdr'):
      hfn = fn + '.hdr'
  else:
      hfn = b + '.hdr'
      
      b_fn = b.strip().split("/")[-1]
      d_type, fn0, fn1 = 4, fn[0], fn[1]
      if( (fn0 == 's') & ((fn1 == '1')|(fn1 == '2')) ):
          # complex scattering matrix format (PolSARPro)
          d_type = 6
      elif(((fn0 == 'R') | (fn0 == 'L')) & ((fn1 == 'R') | (fn1 == 'L'))):
          d_type = 6  # circular polarization cmplx data written by circ.cpp
      else:
          d_type = 4
      write_envi_hdr(fn, nlin, ncol, d_type)
