'''20230527 test integrity of zip files in subfolders
'''
import os
import sys

# check zip
lines = os.popen('find ./ -name "*.zip"').readlines()
lines = [x.strip() for x in lines]

for line in lines:
    cmd = 'zip -T ' + line
    # print(cmd)
    a = os.system(cmd)

# check tar.gz
lines = os.popen('find ./ -name "*.tar.gz"').readlines()
lines = [x.strip() for x in lines]

for line in lines:
    cmd = 'gzip -t ' + line
    # print(cmd)
    a = os.system(cmd)

# check tar
lines = os.popen('find ./ -name "*.tar"').readlines()
lines = [x.strip() for x in lines]

for line in lines:
    cmd = 'tar -tf ' + line
    lines = os.popen(cmd).read().strip()

    if len(lines.lower().split('error')) < 2:
        print(line, "OK")
    else:
        print(line, "ERROR")

