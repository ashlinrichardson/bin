import os
import sys

args = sys.argv

if len(args) < 2:
    print("usage: abn [header file name] # [optional number of bands parameter]  # add band names to header file for rgb.bin")
    sys.exit(1)
if not os.path.exists(args[1]):
    print("Error: abn [header file name]")
    sys.exit(1)

data = open(args[1]).read()

nb = 3
try:
    nb = int(args[2])
except Exception:
    print("Error: number of bands needs to be a number")

if nb == 3:
    open(args[1], "wb").write( (data + "\nband names = {Band 1,\nBand 2,\nBand 3}").encode())
elif nb > 3:
    f = open(args[1], "wb")
    f.write(data.encode())
    f.write("band names = {Band 1,\n".encode())
    for i in range(1, nb - 1):
        f.write(("Band " + str(i + 1) + ",\n").encode())
    f.write(("Band " + str(nb) + "}").encode())
else:
    print("Error: less than 3 bands not supported")

    
