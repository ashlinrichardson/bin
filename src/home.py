# navigate to home folder!
import os
user = os.popen("whoami").read().strip()
cmd = "cd /home/" + user
print(cmd)
a = os.system(cmd)
