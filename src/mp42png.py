'''convert mp4 file to individual png using ffmpeg'''
import os
import cv2
import sys
import math
args = sys.argv

if len(args) < 2 or not os.path.exists(args[1]):
    print("usage: mp42png.py [input mp4 video file] # [optional arg: transpose]") 
    sys.exit(1)
transpose = '' if len(args) < 3 else ' -vf "transpose=clock" '

cap = cv2.VideoCapture(args[1])
frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
print("frame count:", frame_count)

zeros = math.ceil(math.log(frame_count) / math.log(10)) + 1
cmd = "ffmpeg -i "+ args[1] + " " + transpose + " " + args[1] + "_%0" + str(zeros) + "d.png"
print(cmd)
a = os.system(cmd)
