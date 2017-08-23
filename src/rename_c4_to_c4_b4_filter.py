#!/usr/bin/env python
import os
import sys

for fn in open("rs2folders.txt").readlines():
    f = fn.strip()
    f = os.path.abspath(f)
    c4 = f + "/C4/"
    newc4 = f + "/C4b4box/"
    # print c4,newc4
    cmd = "mv " + c4 + " " + newc4
    a = os.system(cmd)
    print a, cmd
