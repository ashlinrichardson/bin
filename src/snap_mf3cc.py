'''assuming we are inside a .dim folder from snap for compact-pol data..
apply Dey et al (2021) mf3cc compact-pol decom'''
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

print("snap_m3fcc # needs to be run from inside a .dim folder for compact-pol data")

''' input files:'''
ins = ['i_RCH2.img', 'i_RCH.img', 'q_RCH2.img', 'q_RCH.img']
for i in ins:
    if not os.path.exists(i):
        err("req'd input file: " + i)
if not os.path.exists("i_RCH.hdr"):
    err("req'd input file: i_RCH.hdr")

run("snap2psp ./")

'''output files from snap2psp'''
bins = ['i_RCH2.bin',  'i_RCH.bin', 'q_RCH2.bin', 'q_RCH.bin']

nrow, ncol = None, None
lines = [x.strip() for x in open('i_RCH.hdr').readlines()]
for line in lines:
    w = [x.strip() for x in line.split("=")]
    if w[0] == 'samples':
        ncol = int(w[1])
    if w[0] == 'lines':
        nrow = int(w[1])

print("nrow", nrow, "ncol", ncol)
