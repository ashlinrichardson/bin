import os
import sys
sep = os.path.sep
folders = [x.strip() for x in os.popen("ls -1 -d *SLC").readlines()]
snap = '/home/' + os.popen('whoami').read().strip() + '/snap/bin/gpt'
for p in folders:
    if os.path.abspath(p) == os.path.abspath('.'):
        continue

    in_1 = p + sep + 'manifest.safe'
    in_2 = p + sep + 'tc.dim'
    c1 = ' '.join([snap,
                  'Terrain-Correction',
                  '-PoutputComplex=true',
                  '-PnodataValueAtSea=false',
                  # '-PsaveLayoverShadowMask=true',
                  in_1,
                  '-t ' + in_2])
    
    in_3 = p + sep + 'b5.dim'
    c2 = ' '.join([snap,
                  'Polarimetric-Speckle-Filter',
                  '-PfilterSize=5',
                  in_2,
                  '-t',
                  in_3])
    print(c1)
    a = os.system(c1)
    if a != 0:
        print("Error:"); sys.exit(1)
    print(c2)
    a = os.system(c2)
    if a != 0:
        print("Error:"); sys.exit(1)
