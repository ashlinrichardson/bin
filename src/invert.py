import os
result = os.popen("xcalib -invert -alter 2>&1").read().strip()

if len(result.split("not found")) > 1:
    print("Attempting to install xcalib, when complete pls. try again..")
    a = os.system("sudo apt install xcalib")
