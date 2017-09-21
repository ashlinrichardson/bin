#!/usr/bin/env python
'''20170907 by a. richardson: mv from dev to tst and
- create backup before overwrite
- apply unix2 dos if xml'''
import os
import sys
import time
from os.path import expanduser
home = expanduser("~")


def error(m):
    print('Error: ' + m)
    sys.exit(1)

v, args = '/Volumes/', sys.argv
if len(args) < 2:
    error("cpd.py: [filename in present path]")

# present directory
pwd = os.path.abspath(os.path.normpath(os.getcwd())) + '/'
print 'pwd', pwd

# make sure file exists
fn = args[1]
if not os.path.exists(fn):
    error('cannot find file: ' + str(fn))

# print source file name
fn = os.path.abspath(os.path.normpath(fn))
print 'fn', fn

# assert source file and present dir
if(fn[0:17] != v + 'envwwwd/'):
    error(str([fn[0:17]]))
if(pwd[0:17] != v + 'envwwwd/'):
    error(str([pwd[0:17]]))

# dest/ target for source file
dst = os.path.abspath(v + 'envwwwt/' + fn[17:])
print 'ds', dst


def p(s):
    s = str(s)
    if len(s) == 1:
        return '0' + s
    return s

# time stamp
t = time.localtime()
ts = (str(t.tm_year) + p(t.tm_mon) + p(t.tm_mday) + '_' +
      p(t.tm_hour) + p(t.tm_min) + p(t.tm_sec))
print "ts", ts

# create backup if over-writing
ard = os.path.abspath(home) + '/' + '.archive/'
# create backup folder (if does not yet exist)
try:
    os.stat(ard)
except:
    os.mkdir(ard)

ph_bk = ard + 'files.txt'
files = []
try:
    # load the index
    files = open(ph_bk).read().strip().split('\n')
    for i in range(0, len(files)):
        files[i] = files[i].strip()
    c = {}
    # check no duplicates:
    for i in range(0, len(files)):
        if not files[i] in c:
            c[files[i]] = 1
        else:
            c[files[i]] += 1
    for ci in c:
        if c[ci] != 1:
            error('entry *' + str(c[ci]))
except:
    files = [dst]

if dst not in files:
    files.append(dst)

# if entry is new, add it to the register
open(ph_bk, "wb").write("\n".join(files))

# now, determine the folder backup name..
ci = 0
for i in range(0, len(files)):
    ci = i if files[i] == dst else ci
print "folder_i", ci

folder = ard + str(ci)
if os.path.exists(folder) and not os.path.isdir(folder):
    error('path exists but is not folder: ' + folder)

# create backup folder (if does not yet exist)
try:
    os.stat(folder)
except:
    os.mkdir(folder)

nexti_f = folder + '/next_i'
nexti = 0
if os.path.exists(nexti_f):
    if os.path.isdir(nexti_f):
        error('path exists but is folder: ' + nexti_f)
    try:
        nexti = int(open(nexti_f).read().strip())
    except:
        nexti = 0

nextis = "%012d" % nexti
print nextis

nexti += 1
open(nexti_f, "wb").write(str(nexti))

arc_fn = folder + '/' + nextis + '_' + ts
arcfn_fn = folder + '/' + nextis + '_' + ts + '.fn'
arcfn_nxt = folder + '/' + nextis + '_' + ts + '.nxt'
print "arcfn_fn", arcfn_fn

# save the name of the file we're pushing to..
open(arcfn_fn, "wb").write(dst)

# the version to push, save it with the entry
cmd = 'cp -v ' + fn + ' ' + arcfn_nxt
print cmd
a = os.system(cmd)
if a != 0:
    error('command failed: ' + cmd)

print "arc_fn", arc_fn

if os.path.exists(arc_fn):
    error('file already exists: ' + arc_fn)

cmd = 'cp -v ' + dst + ' ' + arc_fn
print cmd
a = os.system(cmd)
if a != 0:
    error('command failed: ' + cmd)

cmd = 'cp -v ' + fn + ' ' + dst
a = os.system(cmd)
if a != 0:
    error('command failed: ' + cmd)

print cmd, 'success'
