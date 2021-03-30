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


count  = {}
for root, dirs, files in os.walk(os.getcwd()):
    root = os.path.normpath(root)
    for f in files:
        print(root + f)
        leaf = f.split(os.path.sep)[-1]
        print(leaf)
