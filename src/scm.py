#!/usr/bin/env python
'''integrated Shane-Cloude style visualization method (from S2 matrix data):
s11.bin, s12.bin, s21.bin, s22.bin

reimplemented 20170605 from 20090624 original implementation at CFS AFT

e.g.,can run the following 5 unix/linux commands:

  wget http://mdacorporation.com/sample_data/RS2_OK76385_PK678063_DK606752_FQ2_20080415_143807_HH_VV_HV_VH_SLC.zip
  unzip RS2_OK76385_PK678063_DK606752_FQ2_20080415_143807_HH_VV_HV_VH_SLC.zip
  mkdir s2
  ers RS2_OK76385_PK678063_DK606752_FQ2_20080415_143807_HH_VV_HV_VH_SLC s2
  rm -rf RS2_OK76385_PK678063_DK606752_FQ2_20080415_143807_HH_VV_HV_VH_SLC*
  mkdir scm
  scm s2 scm


'''
import os
import sys
import time
import string
from fl0w3r import error, chkdir, normpath, run

args = sys.argv
if len(args) < 8:
    msg = 'scm.py: image enhancement technique based on a method of Shane '
    msg += 'Cloude. By Ash Richardson, June 24, 2009-- reimplemented 20170605'
    msg += 'Usage: scm4 [input directory] [output directory] [filter type] '
    msg += '[filter size] [faraday correction {yes,no}] [alpha \in'
    error(msg + ' {1, 2, 3, 4}] [vertical multilook factor]')
    # prev. versions: mappingf in ["sch", "sche", "sc1", "sc2"]

# check input and output directories valid
in_dir, out_dir = args[1], args[2]
if not chkdir(in_dir):
    error('input directory not found: ' + in_dir)
if not chkdir(out_dir):
    error('output directory not found: ' + out_dir)
in_dir, out_dir = normpath(in_dir), normpath(out_dir)

# filter type
filter_type, filter_cmds = args[3], {'gauss': 'g4', 'box': 'b4', 'lee': 'l4'}
filter_cmd = filter_cmds[filter_type] if filter_type in filter_cmds else None
if not filter_cmd:
    error('invalid filter type: allowed filters: ' + str(allowed))

# filter size
filter_size = int(args[4])
if filter_size < 0:
    error('positive filter size required')

# faraday rotation correction? yes/no
use_faraday = True if args[5] == "yes" else False

# selection from 1, 2, 3, or 4
alpha_select = int(args[6])
if alpha_select < 1 or alpha_select > 4:
    error('alpha_select \in {1,2,3,4}')

# row-coord multilook factor
multi_look = int(args[7])

# histogram trim factor
htrim_factor = 2.
if htrim_factor <= 0. or htrim_factor >= 100.:
    error('htrim factor should be between 0 and 100')

# intermediary folders
fr_dir = (out_dir[:-1] + '_fr/') if use_faraday else in_dir
t4_dir, fl_dir = out_dir[:-1] + '_fr_t4/', out_dir[:-1] + '_fr_T4_fl'
for tf in [fr_dir, t4_dir, fl_dir]:
    run('mkdir -p ' + tf)
run('cp -v ' + in_dir + 'config.txt ' + fr_dir)

if use_faraday:
    run('fra ' + in_dir + ' ' + fr_dir)

# convert s2 matrix to t4 matrix
run('s2cv ' + fr_dir + ' ' + t4_dir + ' T4 ' + str(multi_look) + ' 1')

# filtering/smoothign data:
run(filter_cmd + ' ' + t4_dir + ' ' + fl_dir + ' ' + str(filter_size))

# mapping
run(mappingf + ' ' + fl_dir + ' ' + out_dir + ' ' + str(0) + ' ' +
    str(alpha_select) + ' ' + str(1.5) + ' 0')

# copy config file
run('cp ' + t4_dir + '/config.txt ' + out_dir)

# create rgb mapping
run('rgbtrim ' + out_dir)
