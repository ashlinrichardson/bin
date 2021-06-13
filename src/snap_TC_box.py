import os
import sys
sep = os.path.sep

def err(m):
    print("Error: " + m); sys.exit(1)

def run(c):
    print(c)
    a = os.system(c)
    if a != 0:
        err('command failed: ' + c)


folders = [x.strip() for x in os.popen("ls -1 -d *SLC").readlines()]
snap = '/home/' + os.popen('whoami').read().strip() + '/snap/bin/gpt' # assume we installed snap
ci = 1

for p in folders:
    print('*** processing ' + str(ci) + ' of ' + str(len(folders)))
    if os.path.abspath(p) == os.path.abspath('.'):
        continue

    in_1 = p + sep + 'manifest.safe'  # input
    in_2 = p + sep + 'tc.dim'  # terrain corrected output
    c1 = ' '.join([snap,
                  'Terrain-Correction',
                  '-PoutputComplex=true',
                  '-PnodataValueAtSea=false', # '-PsaveLayoverShadowMask=true',
                  in_1,
                  '-t ' + in_2])
    
    in_3 = p + sep + 'b5.dim'  # box filtered output
    c2 = ' '.join([snap,
                  'Polarimetric-Speckle-Filter',
                  '-PfilterSize=5',
                  in_2,
                  '-t', in_3])  # -t is for output file
    run(c1)
    run(c2)
