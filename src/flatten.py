'''take all the files in the file system hierarchy subordinate to this node,
and place them in the present directory

do not perform that if any file collision would occur

e.g., flatten
for:

./a/b.txt
./c/d.txt

would produce:
> ls

b.txt c.txt'''

import os
import sys
count, paths = {}, {}
for root, dirs, files in os.walk(os.getcwd()):
    root = os.path.normpath(root) + os.path.sep
    for f in files:
        leaf = f.split(os.path.sep)[-1]
        if leaf not in count:
            count[leaf] = 0
        count[leaf] += 1
        paths[root + f] = leaf

stop = False
for leaf in count:
    if count[leaf] > 1:
        stop = True
        print("error: flatten would collide files with name: " + leaf)

if stop:
    sys.exit(1)

for f in paths:
    cmd = "mv -v " + f + " " + paths[f]
    print(cmd)
    a = os.system(cmd)
