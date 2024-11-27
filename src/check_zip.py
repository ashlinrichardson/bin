'''20230527 test integrity of zip files in subfolders
'''
import os
import sys

lines = os.popen('find ./ -name "*.zip"').readlines()
lines = [x.strip() for x in lines]

for line in lines:
    cmd = 'zip -T ' + line
    print(cmd)
    a = os.system(cmd)

# check tar.gz
lines = os.popen('find ./ -name "*.tar.gz"').readlines()
lines = [x.strip() for x in lines]

for line in lines:
    cmd = 'gzip -t ' + line
    print(cmd)
    a = os.system(cmd)





