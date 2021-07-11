'''assuming we are inside a .dim folder from snap for compact-pol data..
apply Dey et al (2021) [1] mf3cc compact-pol decom'''
import os
import sys
args = sys.argv
sep = os.path.sep

def err(m):
    print("Error: " + m); sys.exit(1)

def run(c):
    print(c)
    a = os.system(c)
    if a != 0:
        err("command failed:\n\t" + c)

print("snap_m3fcc # optional param: window size: default 9 # needs to be run from inside a .dim folder for compact-pol data")
nwin = 9
if len(args) > 1:
    nwin = int(args[1])

''' input files:'''
ins = ['i_RCH2.img', 'i_RCH.img', 'q_RCH2.img', 'q_RCH.img']
for i in ins:
    if not os.path.exists(i):
        err("req'd input file: " + i)
if not os.path.exists("i_RCH.hdr"):
    err("req'd input file: i_RCH.hdr")

# swap byte order from european to american convention
run("snap2psp ./")

'''output files from snap2psp'''
bins = ['i_RCH2.bin',  'i_RCH.bin', 'q_RCH2.bin', 'q_RCH.bin']

# get image dims, will need this to generate C2 matrix cf [2]
nrow, ncol = None, None
lines = [x.strip() for x in open('i_RCH.hdr').readlines()]
for line in lines:
    w = [x.strip() for x in line.split("=")]
    if w[0] == 'samples':
        ncol = int(w[1])
    if w[0] == 'lines':
        nrow = int(w[1])

# convert to ENVI type-6 format
run('convert_iq_to_cplx i_RCH.bin q_RCH.bin ch.bin')
run('convert_iq_to_cplx i_RCH2.bin q_RCH2.bin cv.bin')

print("nrow", nrow, "ncol", ncol)

# convert to C2 matrix cf [1]
args = ['cp_2_t2', 
        str(nrow),
        str(ncol),
        'ch.bin',
        'cv.bin']
run(' '.join(args))


# rename files..
run('mv T11.bin C11.bin')
run('mv T22.bin C22.bin')
run('mv T12_real.bin C12_real.bin')
run('mv T12_imag.bin C12_imag.bin')

# now run the decom
run('mf3cc ./ ' + str(nwin))



'''
[1] Dey et al S. Dey, A. Bhattacharya, D. Ratha, D. Mandal and A. C. Frery, "Target Characterization and Scattering Power Decomposition for Full and Compact Polarimetric SAR Data," in IEEE Transactions on Geoscience and Remote Sensing, doi: https://doi.org/10.1109/TGRS.2020.3010840.

[2] Cloude et al "Compact Decomposition Theory" IEEE GRSL 2011
'''
