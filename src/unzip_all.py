#!/usr/bin/env python
from misc import run

print("Extracting all zip files in current directory...")
lines = os.popen("ls -1 *.zip").readlines()

for line in lines:
	f = os.path.abspath(line.strip())
	run("unzip -e " + f)
