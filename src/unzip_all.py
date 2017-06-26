#!/usr/bin/env python
from fl0w3r import run

print("Extracting all zip files in current directory...")
lines = os.popen("ls -1 *.zip").readlines()

for line in lines:
	f = os.path.abspath(line.strip())
	run("unzip -e " + f)
