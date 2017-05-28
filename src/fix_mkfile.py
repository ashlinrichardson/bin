#!/usr/bin/env python2.7
''' by ashlin richardson 20160524

For annoying "Missing separator" errors: fix c/c++ Makefile
  by replacing each string of consecutive whitespace characters, with
  a single tab character.'''
import os
import sys
from shutil import copyfile

if not os.path.exists("Makefile"):
    print "Error: could not find file: ./Makefile"
    sys.exit(1)

# make a backup
copyfile("Makefile", "Makefile.bak")

# read in Makefile
lines = open("Makefile").readlines()

# write the result back to Makefile
f = open("Makefile", "wb")

# go through the lines of the file
nlin = len(lines)
for j in range(0, nlin):
    line, write_back = lines[j].rstrip(), ""
    for i in range(0, len(line)):
        char, to_write = line[i], ""
        if(char.isspace()):
            if(write_back != ""):
                if(not(write_back[len(write_back) - 1] == '\t')):
                    to_write += '\t'
                else:
                    print "skip redundant whitespace char, line: " + str(j + 1)
            else:
                to_write += '\t'
        else:
            to_write += char
        write_back += to_write
    f.write(write_back)
    if(j < nlin - 1):
        f.write("\n")
f.close()
