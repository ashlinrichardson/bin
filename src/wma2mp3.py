#!/usr/bin/env python
import os
import sys
from fl0w3r import args, run, error, normpath

if len(args) < 2:
    error("wma2mp3 [folder]\n\t convert all wma files in folder to mp3")

folder = normpath(args[1])
cmd = "find " + folder + '  -name "*.wma"'
a = os.popen(cmd).readlines()
n = len(a)
ci = 0

for line in a:
    ci = ci + 1
    print("=====/n=====/n=====/n========CONVERTING "+str(ci)+" of "+str(n))
    mp3 = line.strip().strip("wma") + "mp3"
    run("rm -f " + '"' + mp3 + '"')
    run("ffmpeg -i " + '"' + line.strip() + '"' + " -acodec libmp3lame " +
        "-ab 128k " + '"' + mp3 + '"')
    run("open " + '"' + mp3 + '"')
