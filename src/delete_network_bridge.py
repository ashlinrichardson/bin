#!/usr/bin/env python3
'''20230424 delete network bridge e.g. found by ifconfig'''
import os
import sys

def run(c):
    print(c)
    return os.system(c)

bridges = None
if len(sys.argv) > 1:
    bridges = [sys.argv[1]]

else:
    lines = [x.strip().split() for x in os.popen('brctl show').readlines()]
    bridges = [x[0] for x in lines]

for b in bridges:
    run('sudo ip link set ' + x + ' down')
    run('sudo brctl delbr ' + x)
