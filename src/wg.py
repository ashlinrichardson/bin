# webget-ish thing.. 20211026
import os
import re
import sys
import urllib.request
args = sys.argv
arg = args[1]

import xml.etree.ElementTree as ET

sep = os.path.sep 
key = arg.strip().split(sep)[-1]

with urllib.request.urlopen(arg) as reply:
    reply = reply.read().decode('utf-8') # should have infix operator for this!
    links = re.findall("href=[\"\'](.*?)[\"\']", reply)
    for link in links:
        if len(link.split(key)) > 1:
            print([link])
