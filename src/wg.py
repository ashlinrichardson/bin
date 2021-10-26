# webget-ish thing.. 20211026
import os
import sys
import urllib2
args = sys.argv
with urllib2.request.urlopen(args[1]) as reply:
        print(reply)
sys.exit(1)




