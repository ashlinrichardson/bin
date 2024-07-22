#!/usr/bin/env python
import os
import sys
from misc import run
print("Extracting all zip files in current directory...")
lines = os.popen('find ./ -name "RS2*SLC"').readlines()
cd = os.getcwd()
print("cd " + cd)

for line in lines:
	  f = os.path.abspath(line.strip())
	  os.chdir(f)
	  run("ers ./ ./")
	  os.chdir(cd)
