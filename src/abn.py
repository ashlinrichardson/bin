import os
import sys

args = sys.argv

if len(args) < 2:
    print("usage: abn [header file name] # add band names to header file for rgb.bin")
    sys.exit(1)
if not os.path.exists(args[1]):
    print("Error: abn [header file name]")
    sys.exit(1)

data = open(args[1]).read()

open(args[1], "wb").write( (data + "\nband names = {Band 1,\nBand 2,\nBand 3}").encode())
