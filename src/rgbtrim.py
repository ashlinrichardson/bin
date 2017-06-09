#!/usr/bin/env python
import os
import sys
from fl0w3r import error, run, chkdir

args = sys.argv
if len(args) < 2:
    error('rgbtrim: usage: rgbtrim [input directory]')

# check output folder
o_dir = args[1]
if not chkdir(o_dir):
    error('could not find output folder: ' + o_dir)

# histogram trim the R,G,B channels
run('htrim ' + o_dir + ' ' + o_dir + '/R.bin' + ' 1 1 ' + o_dir + '/Rtrim.bin')
run('htrim ' + o_dir + ' ' + o_dir + '/G.bin' + ' 1 1 ' + o_dir + '/Gtrim.bin')
run('htrim ' + o_dir + ' ' + o_dir + '/B.bin' + ' 1 1 ' + o_dir + '/Btrim.bin')

# overwrite the original R,G,B files
run('mv ' + o_dir + '/Rtrim.bin ' + o_dir + '/R.bin')
run('mv ' + o_dir + '/Gtrim.bin ' + o_dir + '/G.bin')
run('mv ' + o_dir + '/Btrim.bin ' + o_dir + '/B.bin')

# create a bmp file
run('bin2bmp ' + o_dir + ' ' + o_dir + 'out.bmp')
