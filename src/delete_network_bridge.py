#!/usr/bin/env python3
'''20230424 delete network bridge e.g. found by ifconfig'''
import os
import sys

def run(c):
    print(c)
    return os.system(c)

x = sys.argv[1]
run('sudo ip link set ' + x + ' down')
run('sudo brctl delbr ' + x)
