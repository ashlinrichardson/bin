#!/usr/bin/env python3
'''setup RAM disk on Ubuntu. Default to using 50% of total RAM 20220406
Example:
  python3 ramdisk.py # mounts ramdisk at /ram
  python3 ramdisk.py 1 # dismounts ramdisk
'''
import os
import sys
import math
args = sys.argv
exists = os.path.exists

def err(m):
    print("Error: ", m); sys.exit(1)

def run(c):
    print(c); return os.system(c)

def mounted():
    return len(os.popen('df -h | grep /ram').readlines()) > 0

print('python3 ramdisk.py [optional argument: unmount]')
if mounted():
    if len(args) > 1:
        run('sudo umount /ram')
    else:
        err('ramdisk already mounted')

w = os.popen('cat /proc/meminfo  | grep MemTotal').read().strip().split()
if w[0] != 'MemTotal:':
    err('expected: MemTotal:')
if w[2] != 'kB':
    err('expected: kB')

k = int(w[1]) # total RAM
k = int(math.floor(k / 2)) # half of RAM  

if not exists('/ram'):
    run('sudo mkdir /ram') # run('sudo umount /ram')
run('sudo mount -t tmpfs -o rw,size=' + str(k) + 'k tmpfs /ram')

if mounted():
    print('Congrats: ram disk mounted at /ram')
else:
    err('ramdisk at /ram failed to mount')
