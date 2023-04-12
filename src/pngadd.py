import os
import sys
import numpy as np
from PIL import Image

lines = [x.strip() for x in os.popen("ls -1 *.png").readlines()]

print(lines[0])
im = np.array(Image.open(lines[0]))
im = im.astype('float')
for i in range(1, len(lines)):
    print(lines[i])
    x = np.array(Image.open(lines[i]))
    x = x.astype('float')
    im += x

im = im * 255. / np.amax(im)
im = im.astype(np.uint8)

x = Image.fromarray(im)
x.save("sum.png")
