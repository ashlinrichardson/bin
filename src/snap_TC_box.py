import os
import sys
sep = os.path.sep
exist = os.path.exists

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
    
    if not exist(in_2):
        run(c1)
    
    if not exist(in_3):
        run(c2)
    
    r_f = p + '_rgb.bin'
    r_h = p + '_rgb.hdr'
    hf = in_3[:-3] + 'data' + sep + 'T11.hdr'

    if not exist(r_h):
        run('cp ' + hf + ' ' + r_h)

    dat = open(r_h).read().strip()
    dat = dat.replace("bands = 1", "bands = 3")
    dat = dat.replace("band names = { T11 }", "band names = {red, \ngreen,\nblue}")
    open(r_h, 'wb').write(dat.encode())  # write revised header

    if not exist(r_f):
        c = 'cat'
        t = in_3[:-3] + 'data' + sep + 'T'
        for i in ['22.bin', '33.bin', '11.bin']:
            ti = t + i
            if not exist(ti):
                run('sbo ' + (ti[:-3] + 'img') + ' ' + ti + ' 4')
            c += (' ' + t + i)
        c += ' > ' + r_f 
        run(c)
