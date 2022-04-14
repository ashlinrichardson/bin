''' 20220413 iv.py: feed parameters to imv in sequence
e.g.
find ./ -name "*swir.bin_20m.bin" | iv
'''
import os
import sys
sep = os.path.sep
lines = []
for line in sys.stdin:
    lines.append(line.strip().replace('./', ''))
    print('imv ' + line.strip())
    a = os.system('imv ' + line.strip())
