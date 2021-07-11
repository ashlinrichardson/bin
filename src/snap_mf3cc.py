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

if len(args) < 2:
    print("snap_m3fcc")

''' input files:'''

ins = ['i_RCH2.img', 'i_RCH.img', 'q_RCH2.img', 'q_RCH.img']
for i in ins:
    if not os.path.exists(i):
        err("req'd input file: " + i)

run("snap2psp ./")


