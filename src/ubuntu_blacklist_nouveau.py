'''"blacklist" default graphics driver on ubuntu 18

use this to install graphics drivers, e.g. for using any graphics hardware for any purpose incl. GPGPU

https://linuxconfig.org/how-to-disable-nouveau-nvidia-driver-on-ubuntu-18-04-bionic-beaver-linux
'''
# from flow3r import err, run
import os
import sys

def err(m):
    print("Error: " + m)
    sys.exit(1)

def run(c):
    print(str([c]))
    a = os.system(c)
    if a != 0:
        err("command exited with nonzero code")

run('sudo bash -c "echo blacklist nouveau > /etc/modprobe.d/blacklist-nvidia-nouveau.conf"')
run('sudo bash -c "echo options nouveau modeset=0 >> /etc/modprobe.d/blacklist-nvidia-nouveau.conf"')
run('cat /etc/modprobe.d/blacklist-nvidia-nouveau.conf')
run('sudo update-initramfs -u')
run('sudo reboot')
