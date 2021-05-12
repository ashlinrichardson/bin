'''20210512 fix_mac_webcam.py

fix a mac webcam when you get "there is no connected camera" error'''

import os
print("stopping drivers..")
a = os.system("sudo killall VDCAssistant")
a = os.system("sudo killall AppleCameraAssistant")
print("now restart your app that used the webcam..")
