#!/usr/bin/env python
'''integrated Shane-Cloude style visualization method (from S2 matrix data): s11.bin, s12.bin, s21.bin, s22.bin

reimplemented 20170605 from 20090624 original implementation at CFS AFT

e.g., can run the following 5 unix/linux commands:

  wget http://mdacorporation.com/sample_data/RS2_OK76385_PK678063_DK606752_FQ2_20080415_143807_HH_VV_HV_VH_SLC.zip
  unzip RS2_OK76385_PK678063_DK606752_FQ2_20080415_143807_HH_VV_HV_VH_SLC.zip
  mkdir s2
  ers RS2_OK76385_PK678063_DK606752_FQ2_20080415_143807_HH_VV_HV_VH_SLC s2
  rm -rf RS2_OK76385_PK678063_DK606752_FQ2_20080415_143807_HH_VV_HV_VH_SLC*
  mkdir scm
  scm.py ./s2/ ./scm/ box 5 yes 3 1
'''
import os
import sys
import time
import string
exist = os.path.exists
from misc import error, chkdir, normpath, run, read_config

me = os.popen('whoami').read().strip()
convert_command = '/home/' + me + '/GitHub/polsarpro/Soft/bin/data_convert/data_convert.exe'
if not exist(convert_command):
    err('expected command location:' + convert_command)

args = sys.argv
if len(args) < 8:
    msg = 'scm.py: image enhancement technique based on a method of Shane '
    msg += 'Cloude. By Ash Richardson, June 24, 2009-- reimplemented 20170605'
    msg += 'Usage: scm4 [input directory] [output directory] [filter type] '
    msg += '[filter size] [faraday correction {yes,no}] [alpha \in'
    error(msg + ' {1, 2, 3}] [vertical multilook factor]')
    # prev. versions: mappingf in ["sch", "sche", "sc1", "sc2"]

# check input and output directories valid
in_dir, out_dir = args[1], args[2]
if not chkdir(in_dir):
    error('input directory not found: ' + in_dir)
if not chkdir(out_dir):
    error('output directory not found: ' + out_dir)
in_dir, out_dir = normpath(in_dir), normpath(out_dir)

# read image shape
nrow, ncol = read_config(in_dir + 'config.txt')
print('nrow', nrow, 'ncol', ncol)

# filter type
filter_type, filter_cmds = args[3], {'gauss': 'g4', 'box': 'b4', 'lee': 'l4'}
filter_cmd = filter_cmds[filter_type] if filter_type in filter_cmds else None
if not filter_cmd:
    error('invalid filter type: allowed filters: ' + str(filter_cmds))

# filter size
filter_size = int(args[4])
if filter_size < 0:
    error('positive filter size required')

# faraday rotation correction? yes/no
use_faraday = True if args[5] == "yes" else False

# selection from 1, 2, 3
alpha_select = int(args[6])
if alpha_select < 1 or alpha_select > 3:
    error('alpha_select \in {1,2,3}')

# row-coord multilook factor
multi_look = int(args[7])

# histogram trim factor
htrim_factor = 2.
if htrim_factor <= 0. or htrim_factor >= 100.:
    error('htrim factor should be between 0 and 100')

# intermediary folders
fr_dir = (out_dir[:-1] + '_fr/') if use_faraday else in_dir
t4_dir, fl_dir = out_dir[:-1] + '_fr_t4/', out_dir[:-1] + '_fr_t4_fl/'
for tf in [fr_dir, t4_dir, fl_dir]:
    if not exist(tf):
        run('mkdir -p ' + tf)

if use_faraday:
    if not exist(fr_dir + 'config.txt'):
        run('cp -v ' + in_dir + 'config.txt ' + fr_dir)
    if not exist(fr_dir + 's11.bin'):
        run('fra ' + in_dir + ' ' + fr_dir)

# convert s2 matrix to t4 matrix
if not exist(t4_dir + 'T11.bin'):
    a = os.system(convert_command + ' -id ' + fr_dir + ' -od ' + t4_dir + ' -iodf S2T4 -nlr 1 -nlc 1 -ssr 1 -ssc 1 -ofr 0 -ofc 0 -fnr ' + str(nrow) + ' -fnc ' + str(ncol) + ' -sym 0')
    # run('s2cv ' + fr_dir + ' ' + t4_dir + ' T4 ' + str(multi_look) + ' 1')

# filtering/smoothign data:
if not exist(fl_dir + 'T11.bin'):
    run(filter_cmd + ' ' + t4_dir + ' ' + fl_dir + ' ' + str(filter_size))

# mappingf
if not exist(out_dir + 'H.bin'):
    run('sch ' + fl_dir + ' ' + out_dir + ' ' + str(0) + ' ' + str(alpha_select) + ' ' + str(1.5) + ' 0')

# copy config file
if not exist(out_dir + 'config.txt'):
    run('cp ' + t4_dir + 'config.txt ' + out_dir)

# create rgb mapping
if not exist(out_dir + 'Rtrim.bin.hdr'):
    run('rgbtrim ' + out_dir)
