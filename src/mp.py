'''20220325 mirror pattern (mp.py)
Save a sub selection of file tree matching a pattern, to a new path

e.g.: mirror patterned subselection of present dir to ramdisk:
    mp *_L2A_EPSG_32610_10m.bin *_L2A_EPSG_32610_10m.hdr ./ /ram/

e.g.: just list files and confirm adequate space for the above:
    mp *_L2A_EPSG_32610_10m.bin *_L2A_EPSG_32610_10m.hdr ./ /ram/ 1


e.g. copy a certain month:
    python3 mp.py *202109*_L2A_EPSG_32610_10m.bin   *202109*_L2A_EPSG_32610_10m.hdr ./ /ram/
'''
N_THREAD = 2 # number of threads for parallel copy operation
import os
import sys
import multiprocessing as mp
args = sys.argv
sep = os.path.sep
exist = os.path.exists
abspath = os.path.abspath

def err(m):
    print('Error:' + m); sys.exit(1)

def run(c):
    print(c)
    return os.system(c)

if len(args) < 4:
    err('python3 mp.py [search pattern](x1-N) [source path] [destination path] [optional arg: listing only (set to 1)]')

def fsize(fn):
    f = open(fn, 'rb')
    f.seek(0, os.SEEK_END)
    size = f.tell();
    f.close()
    return size

def parfor(my_function, my_inputs, n_thread=mp.cpu_count()): # eval fxn in parallel, collect
    if n_thread == 1:
        result = []
        for i in range(len(my_inputs)):
            result.append(my_function(my_inputs[i]))
        return result
    else:
        if n_thread==None:
            n_thread = mp.cpu_count()
        pool = mp.Pool(n_thread)
        result = pool.map(my_function, my_inputs)
        return(result)

src = args[-3 if args[-1] == '1' else -2]
dst = args[-2 if args[-1] == '1' else -1]
src = abspath(src).strip()
dst = abspath(dst).strip()
print('source path:', src)
print('dest   path:', dst)
for i in [src, dst]:
    if not exist(i):
        err('please check path: ' + str(i))
    if not os.path.isdir(i):
        err('not directory: ' + str(i))

print('patterns:')
ptn = args[1: (-3 if args[-1] == '1' else -2)]

for i in range(len(ptn)):
    if True:  # make a guess if no wildcard
        if len(ptn[i].split('*')) == 1:
            ptn[i] = '*' + ptn[i]
            if ptn[i][-4] == '.':
                ptn[i] = ptn[i] + '*'
    # print('  ' + str(ptn[i]))

files = []
for p in ptn:
    cmd = 'find ' + str(src) + ' -wholename "' + p + '"'
    print(cmd)
    x = [x.strip() for x in os.popen(cmd).readlines()]
    for i in x:
        files.append(i)

if len(files) < 1:
    err('no matches')

fs_total = 0
for f in files:
    fs = fsize(f)
    print(fsize(f), ' ', f)
    df = f.replace(src, dst)
    fs_total += (fs - (fsize(df) if exist(df) else 0))
print('total # of files matched:', len(f))

# check available space
y = os.statvfs(dst)
bs = y.f_frsize # block size
fb = y.f_bfree # free blocks
fs = bs * fb # bytes free
gb = 1024 * 1024 * 1024
print('bytes free', fs, '=', round(fs / gb, 2), 'GB')

cmds = []
for f in files:
    f = abspath(f)
    sf = f
    df = f.replace(src, dst)
    c = ' '.join(['rsync -av', sf, df])
    cmds.append(c)
    print(c)

print('bytes free', fs, '=', round(fs / gb, 2), 'GB')
print('bytes    reqd:', fs_total, '=', round(fs_total/gb, 2), 'GB')
if fs < fs_total:
    print('bytes deficit:', fs_total - fs, '=', round((fs_total-fs)/gb, 2), 'GB')
    err('insufficient space to copy files.')

# create necessary folders:
c_cmds = []
for c in cmds:
    w = c.split()
    df = w[-1];
    w = df.split(sep)[:-1]
    dw = sep.join(w)
    for i in range(len(w) + 1):
        wx = sep.join(w[:i])
        if wx == '':
            continue
        r = wx.replace(dst, '').strip()
        df = sep.join(w[:i])
        if r != wx and df != dst: # don't create subpaths of src path
            print('mkdir', df)
            c_cmds += ['mkdir -p ' + df]

if args[-1] != '1':
    parfor(run, c_cmds, N_THREAD)  # make folders
    parfor(run, cmds, N_THREAD)  # copy files
