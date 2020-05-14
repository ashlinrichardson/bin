#!/usr/bin/env python3
'''compile all the entries in the package'''
import os
import sys
from fl0w3r import normpath, run
src_dir = normpath(os.path.dirname(__file__))

# install gsl on Ubuntu machine (still need to implement this for others)
if os.popen('uname -a').read().strip().split()[0] == 'Ubuntu':
    run('sudo apt-get install libgsl0-dev')

pyfiles, cfiles, cppfiles = [], [], []

def files(ext):
    return os.popen('find ' + src_dir + ' -name "*.' + ext + '"').readlines()

if len(sys.argv) < 2:
    pyfiles = files('py')
    cfiles = files('c')
    cppfiles = files('cpp')
else:
    f = sys.argv[1]
    ext = f.split('.')[-1]
    if ext == 'py':
        pyfiles = [f]
    elif ext =='c':
        cfiles = [f]
    elif ext =='cpp':
        cppfiles = [f]
    else:
        print("Error: unsupported file type: " + f)
        sys.exit(1)

cmpl = src_dir + 'compile.py'

for afiles in [pyfiles, cfiles, cppfiles]:
    for f in afiles:
        f = f.strip()
        if(f.split('/')[-1] != '__init__.py'):
            f = normpath(f)
            run(cmpl + ' ' + f)
