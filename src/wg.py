# webget-ish thing.. 20211026
import os
import sys
import urllib.request
args = sys.argv

import xml.etree.ElementTree as ET

with urllib.request.urlopen(args[1]) as reply:
    reply = reply.read() # should have infix operator for this!
    tree = ET.parse(reply)
    root = tree.getroot()
    print(root)
