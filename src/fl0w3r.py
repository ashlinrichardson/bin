#!/usr/bin/env python2.7
'''
'''
import os
import sys
from ansicolor import KRED, KGRN, KNRM, KMAG

args = sys.argv

def terminal_shape():
    # it would be cool to have progress indicators that varied (in shape, accordingly:) with the size of the terminal screen : ) e.g., to fill up the terminal screen
    try:
        rows, columns = os.popen('stty size', 'r').read().split()
        return rows, columns
    except:
        return None, None

# error message and quit
def error(msg):
    print KRED + "Error: " + KGRN + msg.strip() + KNRM
    sys.exit(1)


# printing message
def printw(f):
    print(KMAG + '+' + KRED + 'w ' + KGRN + f.strip() + KNRM)


# check file message
def chkfile(f):
    if not os.path.exists(f):
        error('file not found: ' + f)


# exists
def exists(f):
    return os.path.exists(f)

# check for valid directory
def chkdir(f):
    return os.path.exists(f) and os.path.isdir(f)


# open write mode
def wopen(fn):
    f = open(fn, 'w')
    if(f is None):
        error('no write access to file: ' + fn)
    printw(fn)
    return f


# read lines from a file
def readlines(fn):
    chkfile(fn)
    return open(fn).readlines()


# read lines from a file
def readlines(f):
    return open(f).read().strip().split('\n')


# normalized file path (with slash, if directory)
def normpath(p):
    pp = os.path.abspath(p.strip())
    if os.path.isdir(pp):
        pp += '/'
    return pp


# normalized directory name (file member removed).. Given a file, get a standardized parent directory name
def normdir(p):
    return normpath(os.path.dirname(p))


# file path (not including file name)
def stem(fn):
    return normdir(fn)
    # e.g., stem('scm.py') == normpath(expanduser('~/pwd/')) ish
    

# file name (without path or extension)
def leaf(fn):
    fn = normpath(fn)
    words = fn.split('/')
    n = words[-1].strip()
    if(n.split('.') > 1):
        return n.split('.')[:-1]
    else:
        return n
    # e.g., leaf('scm.py') == 'scm'
    
# extension for a file
def ext(fn):
    words = normpath(fn.strip()).split('.')
    return words[-1] if len(words) > 1 else None
    # e.g., ext('scm.py') == '.py'

# standardized time stamp string
def timestring():
    import time
    t = time.localtime()
    return (('%04d' % t.tm_year) +
            ('%02d' % t.tm_mon) +
            ('%02d' % t.tm_mday) +
            ('%02d' % t.tm_hour) +
            ('%02d' % t.tm_min) +
            ('%02d' % t.tm_sec))

# run a system command
def run(cmd):
    global log_file
    print KGRN + str(cmd).strip() + KNRM
    log_file = open('logfile.bak', 'a')
    log_file.write(KRED + timestring() + KNRM + ' ' + cmd.strip() + '\n')
    log_file.close()
    a = os.system(cmd)
    if(a != 0):
        error('command failed: ' + cmd)
    log_file = open('logfile.bak', 'a')
    log_file.write('\t' + KMAG + timestring() + KNRM + '\n')
    log_file.close()


# assert that GDAL is installed
def require_gdal():
    if os.popen("which gdalinfo").read().strip() == "":
        error('could not find gdal:: gdalinfo')


# data for PolSARPro config file
def config_file(nrow, ncol):
    return "Nrow\n"+str(nrow)+"\n"+"---------\nNcol\n"+str(ncol)+"\n---------\nPolarCase\nbistatic\n---------\nPolarType\nfull"


# write a PolSARPro config file
def write_config(fn, nrow, ncol):
    wopen(fn).write(config_file(nrow, ncol))
  

# read a PolSARPro config file
def read_config(fn):
    fn = normpath(fn)
    if not (exists(fn) and os.path.isfile(fn)):
        fn += '/config.txt'
    chkfile(fn)
    gl = open(fn).readlines()
    nlin, ncol = int(gl[1].strip()), int(gl[4].strip())
    return nlin, ncol


# test
if __name__ == '__main__':
    error("this is an error message")
