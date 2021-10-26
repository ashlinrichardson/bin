# webget-ish thing.. 20211026
import os
import re
import sys
import urllib.request
args = sys.argv
arg, sep = args[1], os.path.sep 
key = arg.strip().strip(sep).split(sep)[-1]

def afind(x):
    return re.findall("href=[\"\'](.*?)[\"\']", x)

def run(x):
    print(x); a = os.system(x); return(a)

with urllib.request.urlopen(arg) as reply:
    reply = reply.read().decode('utf-8') # should have infix operator for this kind!
    links = afind(reply)
    for link in links:
        w = link.split(key)
        if len(w) > 1:
            tgt = arg + w[-1].strip(sep) + sep
            reply = urllib.request.urlopen(tgt).read().decode('utf-8')
            links2 = afind(reply)
            for link2 in links2:
                link2 = link2.strip()
                if link2[-3:] == 'zip':
                    w2 = link2.strip().split(sep)[1]
                    pre = arg.split(w2)
                    tgt2 = pre[0].strip(sep) + link2
                    a = run('wget --continue ' + tgt2)         

