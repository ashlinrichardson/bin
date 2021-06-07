'''take all the files in the file system hierarchy subordinate to this node,
and place them in the present directory

do not perform that for any colliding files.

e.g., flatten
for:

./a/b.txt
./c/d.txt

would produce:
> ls

b.txt c.txt 

20210604 adjusted to compare files with same names, removing redundant ones'''

import os
import sys
count, paths, instances, my_root = {}, {}, {}, os.path.normpath(os.getcwd())

for root, dirs, files in os.walk(os.getcwd()):
    root = os.path.normpath(root) + os.path.sep
    for f in files:
        leaf = f.split(os.path.sep)[-1]
        if leaf not in count:
            count[leaf] = 0
            instances[leaf] = []
        count[leaf] += 1
        instances[leaf] += [root + f]
        paths[root + f] = leaf

stop = False
for leaf in count:
    if count[leaf] > 1:
        # stop = True
        # print("warning: flatten would collide files with name: " + leaf)
        for instance in instances[leaf]:
            instance = instance.strip()
            rl = my_root + os.path.sep + leaf
            rl = rl.strip()
            if (instance != rl) and (os.path.exists(instance) and os.path.exists(rl)) and (int(os.stat(instance).st_size) != int(os.stat(rl).st_size)):
                stop = True
                print("Error: different file sizes!")
                print("\t", instance, os.stat(instance).st_size)
                print("\t", rl, os.stat(rl).st_size)

if stop:
    sys.exit(1)

for f in paths:
    src, dst = os.path.abspath(f), os.path.abspath(paths[f])
    cmd = " ".join(["mv -v", src, dst])
    if (src != dst) and not(os.path.isdir(src) or os.path.isdir(dst)): 
        print(cmd)
        a = os.system(cmd)


for root, dirs, files in os.walk(os.getcwd()):
    root = os.path.normpath(root) + os.path.sep
    for d in dirs:
        d = os.path.abspath(d) + os.path.sep
        cmd = "rmdir " + d
        print(cmd)
        a = os.system(cmd)
