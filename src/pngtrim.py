'''20221207 trim png files in present directory using imagemagick
i.e. remove empty space'''
import os
import sys

def run(c):
    print(c)
    if os.system(c) != 0:
        print("Error: command failed: " + c)
        sys.exit(1)
png = os.popen("ls -1 *.png").readlines()

pns = []
for p in png:
    p = p.strip()
    pt = p + '_trim.png'
    run('convert -trim ' + p + ' ' + pt)
    run('mv -v ' + pt + ' ' + p)
