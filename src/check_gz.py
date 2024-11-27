'''20241127 test integrity of .tar.gz files in subfolders
'''
import os
import sys
# check tar.gz
lines = os.popen('find ./ -name "*.tar.gz"').readlines()
lines = [x.strip() for x in lines]

for line in lines:
    cmd = 'gzip -t ' + line
    print(cmd)
    a = os.system(cmd)





