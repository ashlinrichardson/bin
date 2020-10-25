import os
import sys

# add band name string to end of header, if it doesn't exist, before running imv

args = sys.argv

if len(args) < 2:
    print("Error: im [input binary file name] # invoke cmd-line utility: imv, assuming 3-band files: add band names field to header (if no band names field present)")
    sys.exit(1)

f = args[1]
if not os.path.exists(f):
    print("Error: input file does not exist:" + f)
    sys.exit(1)

hfn = f + '.hdr'
h1 = hfn
if not os.path.exists(hfn):
    if f[-4:] == '.bin':
        hfn = f[:-4] + '.hdr'
if not os.path.exists(hfn):
    print("Could not find header at: " + h1)
    print("Could not find header at: " + hfn)
    sys.exit(1)

print(hfn)

lines = open(hfn).readlines()

bn = False
for i in range(0, len(lines)):
    if len(lines[i].split("band names")) > 1:
        bn = True

if not bn:
    open(hfn, "wb").write(('\n'.join(lines) + '\nband names = {Band 1,\nBand 2,\nBand 3}').encode())

a = os.system("imv " + f)
