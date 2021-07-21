import os
import sys
import imageio

files = [f.strip() for f in os.popen("ls -1 *.png").readlines()]
for f in files:
    im = imageio.imread(f)
    print(im.shape)

