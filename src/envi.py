import cv2
import struct
import numpy as np
from fl0w3r import *
from ansicolor import KRED, KGRN, KNRM, KMAG

'''
@brief interpret envi format header
'''


def read_envi_hdr(fn):
    d = {}
    try:
        # pick out lines of file that resemble 'assignment statements'
        for line in readlines(fn):
            w = line.strip().split('=')
            if(len(w) == 2):
                d[w[0].strip()] = w[1].strip()

        # display dimensions
        print 'nr', d['lines'], 'nc', d['samples'], 'nb', d['bands']
        return d['lines'], d['samples'], d['bands']
    except:
        error('could not parse envi .hdr file: ' + fn)

'''
@brief interpret rows, cols, and number of bands as envi header data
'''


def hdr_d(fn, nr, nc, nb):
    return ['ENVI',
            'description = {' + fn.strip() + '}',
            'samples = ' + str(nc),
            'lines   = ' + str(nr),
            'bands   = ' + str(nb),
            'header offset = 0',
            'file type = ENVI Standard',
            'data type = 4',
            'interleave = bsq',
            'byte order = 0',
            'band names = {}']

'''
@brief interpret image dimensions as envi header file
'''


def write_envi_hdr(fn, nr, nc, nb):
    # open file and write header data
    f = wopen(fn + '.hdr')
    [f.write(d + '\n') for d in hdr_d(fn, nr, nc, nb)]

    # close file and print message
    f.close()
    printw(fn)

'''
@brief interpret envi format binary file with header as cv2 image data
'''


def load_envi_cv2(fn):
    chkfile(fn)
    f = open(fn)
    h_fn = fn + '.hdr'
    if not exists(h_fn):
        if(fn[-4:] == '.bin'):
            h_fn = fn[:-4] + '.hdr'
    if not exists(h_fn):
      error('could not find header file for envi binary file :' + fn)

    nr, nc, nb = read_envi_hdr(h_fn)
    d, nb, nr, nc = None, int(nb), int(nr), int(nc)
    
    if(nb == 3):
        # bgr image
        m = {2: 0, 1: 1, 0: 2}
        d = np.zeros((nr, nc, 3))
        for k in range(0, nb):
            for i in range(0, nr):
                for j in range(0, nc):
                    d[i, j, m[k]] = float(struct.unpack('f', f.read(4))[0])
    elif(nb == 1):
        # intensity image
        d = np.zeros((nr, nc))
        for i in range(0, nr):
            for j in range(0, nc):
                d[i, j] = float(struct.unpack('f', f.read(4))[0])
    else:
        error('loading nbs != 3 not supported')
    return d / 255.

'''
@brief interpret cv2 image data as envi format binary file with header
'''


def write_cv2_envi(d, fn):
    f = wopen(fn)
    h_fn = fn + '.hdr' if (fn[-4:] != '.bin') else (fn[:-4] + '.hdr')
    nr, nc, nb = None, None, None

    if(len(d.shape) == 3):
        # bgr image
        m = {2: 0, 1: 1, 0: 2}
        nr, nc, nb = d.shape
        write_envi_hdr(fn, nr, nc, nb)
        for k in range(0, nb):
            for i in range(0, nr):
                for j in range(0, nc):
                    f.write(struct.pack('f', d[i, j, m[k]]))
    elif(len(d.shape) == 2):
        # intensity image
        nr, nc = d.shape
        write_envi_hdr(fn, nr, nc, 1)
        for i in range(0, nr):
            for j in range(0, nc):
                f.write(struct.pack('f', d[i, j]))
    else:
        # undefined image type
        error('only 1 or 3 bands supported')  

