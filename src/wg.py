# webget-ish thing.. 20211026
import os
import re
import sys
import urllib.request
args = sys.argv

import xml.etree.ElementTree as ET

with urllib.request.urlopen(args[1]) as reply:
    reply = reply.read().decode('utf-8') # should have infix operator for this!
    print(reply)

    links = re.findall("href=[\"\'](.*?)[\"\']", reply)
    for link in links:
        print(link)
