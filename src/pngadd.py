import os
import sys
import numpy as np
from PIL import Image

lines = [x.strip() for x in os.popen("ls -1 *.png").readlines()]

print(lines[0])
im = np.array(Image.open(lines[0]))

for i in range(1, len(lines)):
    print(lines[i])
    im += np.array(Image.open(lines[i]))

im = Image.fromarray(im)
im.save("sum.png")
