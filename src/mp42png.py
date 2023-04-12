'''convert mp4 file to individual png using ffmpeg'''
import os
import cv2
import sys
import math
args = sys.argv

if not os.path.exists(args[1]):
    print("usage: mp42png.py [input mp4 video file]")
    sys.exit(1)

cap = cv2.VideoCapture(args[1])
frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
zeros = math.ceil(math.log(frame_count) / math.log(10)) + 1
cmd = "ffmpeg -i " + args[1] + " " + args[1] + "_%0" + str(zeros) + "d.png"
print(cmd)
a = os.system(cmd)
