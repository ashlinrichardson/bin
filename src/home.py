# navigate to home folder!
import os
user = os.popen("whoami").read().strip()
cmd = "cd /home/" + user
a= os.system(cmd)
