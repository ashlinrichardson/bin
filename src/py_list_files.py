#!/usr/bin/env python
'''list files in a directory
directory is given as first command line arg. at execution
unknown date-- revised 20170826'''
import os
import sys
fileList = []
rootdir = sys.argv[1]

for root, subFolders, files in os.walk(rootdir):
    for file in files:
        fileList.append(os.path.join(root, file))

for filename in fileList:
    print(filename)  # print fileList
