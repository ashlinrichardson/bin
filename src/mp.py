'''20220325 mirror pattern 
mp.py
Save a sub selection of file tree matching a pattern, to a new path
'''
import os
import sys
args = sys.argv
def err(m):
    print('Error:' + m); sys.exit(1)

if len(args) < 4:
    err('python3 mp.py [search pattern](x1-N) [source path] [destination path]')

