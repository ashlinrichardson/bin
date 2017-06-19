#!/usr/bin/env python
'''repeat command at regular interval (in seconds): default 10 seconds.
if first command line argument can be interpreted as integer, override
the default value. use remaining args as command to run. 20170619 AR'''
import os
import sys
import time
from fl0w3r import args, error

default = 10
delay, arg = default, args[1:]

if len(args) > 1:
    try:
        delay = int(args[1])
        arg = args[2:]
    except:
        pass

cmd = ' '.join(arg)
print cmd

while(True):
    a = os.system(cmd)
    time.sleep(delay)
