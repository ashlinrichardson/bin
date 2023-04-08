''' 20220408 s2s.py: sort sentinel2 filenames from Stdin
e.g.:
    find ./ -name "*_10m.bin" | s2s '''
import os
import sys
sep = os.path.sep
lines = []
for line in sys.stdin:
    lines.append(line.strip().replace('./', ''))

X = []
for line in lines:
    ds = None
    w = line.split(sep)
    for i in w:
        if i[:2] == 'S2':
            v = i.split('_')
            ds = v[2]
            X.append([ds, line])
            break
    if ds is None:
        print('Error: Sentinel2 date string not found')
        sys.exit(1)
X.sort()

for x in X:
    print(x[1])
