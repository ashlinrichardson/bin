# supposing float image, arbitrary shape, output mask band
import os
import sys
import math
import numpy as np
args = sys.argv

def err(m):
    print("Error:", m)
    sys.exit(1)

if len(args) < 2: err("usage:\n  isnan [input type4 binary file]")
if not os.path.exists(args[1]): err("failed to open input file: " + args[1])
data = np.fromfile(args[1], '<f4')
for i in range(len(data)):
    data[i] = 1. if (math.isnan(data[i]) or math.isinf(data[i])) else 0. 
of = args[1][:-4] + "_isnan.bin"
print("+w ", of)
data.tofile(of, '', '<f4')
oh = args[1][:-4] + "_isnan.hdr"
print("+w ", oh)
a = os.system("cp " + args[1][:-4] + ".hdr " + oh)
