'''20220411 convert all png files to jpg in parallel'''
import os 
import sys

files = [x.strip() \
         for x in os.popen('ls -1 *.png').readlines()]

for f in files:
    cmd = 'convert ' + f + ' ' + f[:-3] + 'jpg &' 
    print(cmd)
    a = os.system(cmd)
