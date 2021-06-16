import os
import sys
args = sys.argv
sep = os.path.sep

def err(m):
    print("Error: " + str(m)); sys.exit(1)

if len(args) < 3:
    err("multilook_t3.py [input t3 folder] [output t3 folder] [square window size]")

i_d = args[1]
o_d = args[2]

if not os.path.exists(i_d) or not os.path.isdir(i_d):
    err("check input directory: " + i_d)
if not os.path.exists(o_d) or not os.path.isdir(o_d):
    err("check input directory: " + o_d)

ws = None
try:
    ws = int(args[3])
except Exception:
    err("window size must be an int")

T = ["T11.bin",
     "T12_imag.bin",
     "T12_real.bin",
     "T13_imag.bin",
     "T13_real.bin",
     "T22.bin",
     "T23_imag.bin",
     "T23_real.bin",
     "T33.bin"]

''' Usage: multilook [input file] [path to directory containing config.txt file] [output file] [window size]
Note: config.txt file must be present '''
for t in T:
    i_f = i_d + sep + t
    o_f = o_d + sep + t
    cmd = "multilook " + i_f + " " + i_d + " " + o_f + " " + str(ws)
    print(cmd)
