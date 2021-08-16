import os
import sys
args = sys.argv

def err(m):
    print("Error: " + str(m)); sys.exit(1)

if len(args) < 2:
    err("usage:\n\tpython3 unique.py [file name] # output unique lines of file")

lines = list(set([x.rstrip() for x in open(args[1]).readlines()]))
for line in lines:
    print(line)
