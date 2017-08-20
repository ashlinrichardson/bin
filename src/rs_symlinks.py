#!/usr/bin/env python

import os
import sys
ls = os.popen("ls -1 *.zip").readlines()
pwd = os.popen("pwd").read().strip()
f = open("rs2folders.txt", "w")
for l in ls:
    fn = l.strip()
    if fn[:3] == 'RS2' & fn[-4:] == '.zip':
        fns = fn[:-4]
        fnss = fns.strip().split("_")
        newname = fnss[5]
        rs2folder = fns
        linkfolder = newname
        cmd = "ln -s " + rs2folder + " " + linkfolder
        a = os.system(cmd)
        print a, cmd
        f.write(linkfolder + "\n")
f.close()
