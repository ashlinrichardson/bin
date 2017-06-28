#!/usr/bin/env python
import os
import sys

argv = sys.argv

if len(argv) < 3:
    print("save cookies.txt for ASF login site.\n")
    print("saveCookies [USER] [PASS]\n")
    sys.exit(1)

if len(argv) >= 3:
    USER_NAME = sys.argv[1]
    USER_PASS = sys.argv[2]
else:
    print "error: see usage"
    sys.exit(1)

run("wget --save-cookies cookies.txt --post-data='user_name=" + USER_NAME +
    "&user_password=" + USER_PASS +
    "' --no-check-certificate https://ursa.asfdaac.alaska.edu/cgi-bin/login " +
    "-o /dev/null -O /dev/null")
