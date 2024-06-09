import os
import sys
'''20240609 need to update this for recursive find.

copy zip files from one to another.
 files that exist in source folder, but not destination folder, are copied
 files that exist in source folder, and destination folder: their sizes are checked:
     if the zip from the souce folder is larger, we prompt to copy it over the zip in the destination folder'''

args = sys.argv

if len(args) < 3:
    print("Usage: cpzip [input folder] [output folder] # a non-clobber copy! # Need not be just for zips")
    sys.exit(1)
fi = args[1]
fo = args[2]
 

fi = os.path.abspath(fi) + os.path.sep
fo = os.path.abspath(fo) + os.path.sep

zi = os.popen("ls -1 " + fi + "*.zip")
zo = os.popen("ls -1 " + fo + "*.zip")

zi = [z.strip() for z in zi]
zo = [z.strip() for z in zo]

zis = [z.split(os.path.sep)[-1] for z in zi]
zos = [z.split(os.path.sep)[-1] for z in zo]

ziss = set(zis)
zoss = set(zos)

can = ziss.difference(zoss) # zips not in output folder at all
chk = ziss.intersection(zoss) # zips we need to check:

# copy files that aren't at destination folder:
for i in can:
    cmd = "cp -v " + fi + i + " " + fo + i
    a = os.system(cmd)

for i in range(0, len(zis)):
    ii = zis[i].strip()
    if ii in chk:
        src = zi[i]
        dst = fo + ii

        s1, s2 = os.path.getsize(src), os.path.getsize(dst)
        if s1 != s2:
            print(s1, src)
            print(s2, dst)
            cmd = "cp -v " + src + " " + dst
            print(cmd)
            print("Execute above command? (y/n) n to skip")
            inp = input()
            if inp == '' or inp == 'y' or inp == 'Y':
                a = os.system(cmd)
            
