#!/usr/bin/env python
'''Ashlin Richardson (ashy@uvic.ca) 2014 era program revised 20170813
rs2sl.py: create local symbolic links to complicated radarsat2 directory names.
'''
import os
import sys


def yes(msg):
    return(raw_input("%s (y/N) " % msg).lower() == 'y')

cmds = []
fn = os.popen("ls -1 *.zip").readlines()
for i in fn:
    is1 = i.strip().strip(".zip")
    is1s = is1.strip().split("_")
    if len(is1s) < 6:
        continue
    else:
        RS2 = is1s[0]
        MODE = is1s[4]
        DATE = is1s[5]
        dirpath = os.path.abspath(is1)
        if(RS2 != "RS2"):
            continue
        else:
            YEAR = DATE[0:4]
            MONTH = DATE[4:6]
            DAY = DATE[6:8]
            YMD = "./" + YEAR + MONTH + DAY
            source_file = dirpath
            link_name = os.path.abspath(YMD)
            cmd = "ln -s " + source_file + " "+link_name
            cmds.append(cmd)

print "Symbolic link commands to be run..."
for i in cmds:
    print(i)

success = True

if yes("Shall I run these commands?(y/N)"):
    for i in cmds:
        ret = os.system(i)
        if int(ret) != 0:
            print("WARNING: command failed.")
            print("Failed command: " + i)
            success = False

if(success):
    print("All commands executed successfully (return code 0)")
else:
    print("Warning: >=1 command failed (return code != 0)")

# 0 RS2, 04mode, 5 date
