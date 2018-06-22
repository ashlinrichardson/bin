#!/usr/bin/env python
'''20180620: github b4ckup'''
import os
import sys
from urllib2 import urlopen
from fl0w3r import normpath, wopen, error, args, exists
from ansicolor import KYEL, KMAG, KRED, KGRN, KNRM

user = 'ashlinrichardson'
if len(args) > 1:
    user = args[1]

# make local folder to store repo archives
u_dir = "github:" + user
if not exists(u_dir):
    os.mkdir(u_dir)
else:
    # make sure the name isnt' taken by something else
    if not os.path.isdir(u_dir):
        error(u_dir + " is not directory")

# fetch the repo list
url, url_ = 'https://github.com/' + user + '?tab=repositories', None
try:
    url_ = urlopen(url)
    code = url_.code  # expect 200
except:
    error("github user: " + KYEL + user + KNRM + " not found")

lines = url_.read().split('\n')
for line in lines:
    l = line.strip()
    ls = l.split('itemprop="name codeRepository"')
    if len(ls) > 1:
        # this line refers to a repo.. get the repo name
        repo = l.split('itemprop')[0].split('href="')[1]\
                .split('/')[2].split('"')[0]
        arc = 'https://github.com/' + user + '/' + repo + '/archive/master.zip'
        try:
            o_fil = u_dir + '/' + repo + '_master.zip'
            print 'wget ' + KMAG + arc + KNRM
            r_url = urlopen(arc)  # open the url
            f = wopen(o_fil)  # open a file handle
            if not(f):
                error("failed to open file: " + o_fil)
            data = r_url.read()  # pull data from url
            print '\t' + KRED + '+' + KMAG + 'w ' + KGRN + o_fil + KNRM
            f.write(data)  # write to file
        except:
            print("warning: failed to open file:\n\t" +
                  KYEL + arc + KNRM + '\n')
