# webget-ish thing.. 20211026
import os
import re
import sys
import urllib.request
args = sys.argv

import xml.etree.ElementTree as ET

with urllib.request.urlopen(args[1]) as reply:
    reply = reply.read() # should have infix operator for this!
    print(reply.decode())

    links = re.findall("href=[\"\'](.*?)[\"\']", reply)
    print(links[:5])
