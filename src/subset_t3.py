import os
import sys
from misc import read_config, write_config
args, sep = sys.argv, os.path.sep

def err(m):
    print("Error: " + str(m)); sys.exit(1)

if len(args) < 7:
    err("subset_t3.py [input t3 folder] [output t3 folder] [row start] [col start] [nrow_final] [ncol_final]")

i_d, o_d = args[1], args[2]
if not os.path.exists(i_d) or not os.path.isdir(i_d) or not os.path.exists(o_d) or not os.path.isdir(o_d):
    err("check input directory: " + i_d + " or output directory: " + o_d )
ws = None
try: ws = int(args[3])
except Exception: err("window size must be an int")

print(i_d)
cfg = i_d + sep + 'config.txt'
if not os.path.exists(cfg):
    err("PolSARPro config.txt file expected here: " + cfg)
nrow, ncol = read_config(cfg)
print(nrow, ncol)

row_start = int(args[3])
col_start = int(args[4])
row_total = int(args[5])
col_total = int(args[6])
row_end = row_start + row_total - 1
col_end = col_start + col_total - 1

cfg_out = o_d + sep + 'config.txt'
write_config(cfg_out, row_total, col_total)

T = ["T11.bin",
     "T12_imag.bin",
     "T12_real.bin",
     "T13_imag.bin",
     "T13_real.bin",
     "T22.bin",
     "T23_imag.bin",
     "T23_real.bin",
     "T33.bin"]

''' subset_bin [infile] [nrow] [ncol] [outfile] [rowstart] [rowend] [colstart] [colend]
'''

''' Usage: multilook [input file] [path to directory containing config.txt file] [output file] [window size]
Note: config.txt file must be present '''
for t in T:
    i_f = i_d + sep + t
    o_f = o_d + sep + t
    # subset_bin .//T11.bin 624 2304 subs//T11.bin 0 623 0 2303
    cmd = " ".join(['subset_bin', i_f, str(nrow), str(ncol), o_f, str(row_start), str(row_end), str(col_start), str(col_end)])
    print(cmd)
    a = os.system(cmd)
    if a != 0:
        err("command failed")
