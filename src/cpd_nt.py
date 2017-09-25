# $file$ Promote.py
# $descr$ copy from dev to test, backup before overwriting copy target
# $author$ ash richardson
# $date$ 20170923
'''a minimalist versioning system
*** notes: requires envwwwt on Y:, and envwwwd on Z: ***

*** limitation: program will work on the order of one billion times, cumulatively for a given input parameter
 -- the limitation overcome by applying dynamic length string allocation methods ( trivial
    programming exercise suitable to delegate to a student)

tested for robustness by:
    attempting to promote C:\\python27\\NEWS.txt (failed as, not on dev drive Z:\\)
    attempting to promote Y:\\_shared\\xml\\parkinfo.json (failed as, not on dev drive) ***
'''

import sys
def error(m):
    print "Error: " + str(m)
    raw_input('\nplease press ENTER to continue..')
    sys.exit(1)
    
import os
if os.name != "nt":
    error("this version requires the Windows operating system")

import sys, time
from os.path import expanduser

# home folder, file to promote
home, args = expanduser("~") + '\\', sys.argv

if len(args) < 2:
    error('file argument not supplied')
    
fn = os.path.abspath(args[1])  # fully qualified path

# make sure the source path exists (and is a file)
if not(os.path.exists(fn) and os.path.isfile(fn)):
    error('file not found: ' + str(fn))

drive, path = os.path.splitdrive(fn)
path, filename = os.path.split(path)
# print "drive", drive, "path", path, "filename", filename

# associate volume name with drive letter
d = os.popen("wmic logicaldisk get caption,volumename").readlines()
volume_name = {}
path_name = {}
for i in d:
    ii = str(i).split()
    if len(ii) == 2:
        p = os.path.abspath(ii[0])
        if os.path.exists(p):
            volume_name[p[0]], path_name[ii[1]] = ii[1], p[0] 

# print volume_name
# print path_name
# print "drive letter for this file:", drive[0]
# print "volume name for this file:", volume_name[drive[0]]
if volume_name[drive[0]] != 'envwwwd':
    error('this program only defined for files on envwwwd')

# check if envwwwt a key of path_name (dict)
if 'envwwwt' not in path_name:
    error("envwwwt not mounted on Y:")
# check drive letter for envwwwt maps back to a valid drive..
tgt_drv = os.path.abspath(path_name['envwwwt'] + ':')
if not os.path.exists(tgt_drv):
    error("envwwwt not mounted on Y:")

print "source (wwwd): ", fn
# dest/ target for source file
dst = tgt_drv[0] + fn[1:]
print 'target (wwwt): ', dst

def p(s):
    s = str(s)
    if len(s) == 1:
        return '0' + s
    return s

# time stamp
t = time.localtime()
ts = (str(t.tm_year) + p(t.tm_mon) + p(t.tm_mday) + '_' +
      p(t.tm_hour) + p(t.tm_min) + p(t.tm_sec))
# print "timestamp", ts

# create backup if over-writing (archive called, promote)
ard = home + 'promote\\'
# print ard

# create backup folder (if does not yet exist)
if not (os.path.exists(ard) and os.path.isdir(ard)):
    try:
        os.mkdir(ard)
    except:
        error('failed to make directory: ' + ard)

ph_bk = ard + 'files.txt'
# print 'phone book: ' + str(ph_bk)

files = []
try:
    # load the index
    files = open(ph_bk).readlines()
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
# print "folder_i", ci

folder = ard + str(ci) + '\\'
if os.path.exists(folder) and not os.path.isdir(folder):
    error('path exists but is not folder: ' + folder)

# print "folder", str(folder)

# create backup folder (if does not yet exist)
try:
    os.stat(folder)
except:
    os.mkdir(folder)

nexti_f = folder + 'next_i'
nexti = 0
if os.path.exists(nexti_f):
    if os.path.isdir(nexti_f):
        error('path exists but is folder: ' + nexti_f)
    try:
        nexti = int(open(nexti_f).read().strip())
    except:
        nexti = 0

nextis = "%012d" % nexti
# print nextis

nexti += 1
open(nexti_f, "wb").write(str(nexti))

arc_fn = folder + nextis + '_' + ts
arcfn_fn = folder + nextis + '_' + ts + '.fn'
arcfn_nxt = folder + nextis + '_' + ts + '.nxt'
# print "arcfn_fn", arcfn_fn

# save the name of the file we're pushing to..
open(arcfn_fn, "wb").write(dst)

import shutil
try:
    # save the version to push
    print('cp ' + fn + ' ' + arcfn_nxt)
    shutil.copyfile(fn, arcfn_nxt)
except:
    error('failed to copy file', fn, arcfn_nxt)

# sanity check: make sure archive doesn't already exist    
if os.path.exists(arc_fn):
    error('file already exists: ' + arc_fn)

# backup the target file (if exists) before overwriting
if os.path.exists(dst) and os.path.isfile(dst):
    try:
        print('cp ' + dst + ' ' + arc_fn)
        shutil.copyfile(dst, arc_fn)
    except:
        error('failed to copy file', dst, arc_fn)

# copy the source file to the target location
try:
    print('cp ' + fn + ' ' + dst)
    shutil.copyfile(fn, dst)
except:
    error('failed to copy file', fn, dst)

print 'success'

# pause so we can see the output
d = raw_input('\nplease press ENTER to continue..'); sys.exit(1)    

