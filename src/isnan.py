# supposing float image, arbitrary shape, output mask band
import os
import sys
import math
import numpy as np
args = sys.argv

if len(args) < 2:
    print("Error: usage:\n  isnan [input type4 binary file]"); sys.exit(1)

if not os.path.exists(args[1]): 
    print("Error: failed to open input file: ", args[1]); sys.exit(1)

data = np.fromfile(args[1], '<f4')

for i in range(0, len(data)):
    if math.isnan(data[i]) or math.isinf(data[i]):
        data[i] = 1.
    else:
        data[i] = 0.

of = args[1][:-4] + "_isnan.bin"
print("+w ", of)
data.tofile(of, '', '<f4')
oh =args[1][:-4] + "_isnan.hdr"

a = os.system("cp " + args[1][:-4] + ".hdr " + oh)
