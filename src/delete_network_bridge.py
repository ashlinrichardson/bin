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
    bridges = []
    lines = [x.strip().split() for x in os.popen('brctl show').readlines()][1:]
    for line in lines:
        print(line)
        bridges += [line[0]]

for b in bridges:
    run('sudo ip link set ' + b + ' down')
    run('sudo brctl delbr ' + b)
