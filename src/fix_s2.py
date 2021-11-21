'''add folders expected by sen2cor'''
import os
import sys
args = sys.argv
sep = os.path.sep

f = args[1]

def mk(f):
    if not os.path.exists(f):
        print('mkdir', f)
        os.mkdir(f)

mk(f + sep + 'AUX_DATA')
mk(f + sep + 'HTML')



