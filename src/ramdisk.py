'''setup ubuntu RAMDISK. Default to using 50% of total RAM'''
import os
import sys
import math
exists = os.path.exists

def err(m):
    print("Error: ", m); sys.exit(1)

def run(c):
    print(c)
    return os.system(c)

w = os.popen('cat /proc/meminfo  | grep MemTotal').read().strip().split()

if w[0] != 'MemTotal:':
    err('expected: MemTotal:')
if w[2] != 'kB':
    err('expected: kB')
k = int(w[1])
k = int(math.floor(k/2))

if not exists('/ram'):
    run('sudo mkdir /ram')

run('sudo umount /ram')
run('sudo mount -t tmpfs -o rw,size=' + str(k) + 'k tmpfs /ram')
