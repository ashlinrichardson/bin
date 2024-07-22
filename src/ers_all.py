#!/usr/bin/env python
'''run ers on a bunch of folders'''
import os
import sys
from misc import run

ls = os.popen('ls -1 *.zip').readlines()
pwd = os.popen('pwd').read().strip()

for l in ls:
	fn = l.strip();
	if((fn[:3] == 'RS2') & (fn[-4:] == '.zip')):
		rs2folder = pwd + "/" + fn[:-4] + "/"
		print rs2folder
		run("ers " + rs2folder + " " + rs2folder)
