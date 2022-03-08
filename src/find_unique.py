'''20220307 Group results of find command, into equivalence classes
(by file name not incl. path)

Optional argument: whether to display the locations where each filename exist
(could also do this by piping into a "unique" command..
..next time actually should implement unique.cpp to take stdin for unix pipe)

Usage:
    python3 find_unique.py [pattern e.g. PRS*zip for prisma data]'''
import os
import sys
args = sys.argv
lines = [x.strip()
         for x in os.popen('find ./ -name "' +
                           args[1] +
                           '"').read().strip().split('\n')]

for line in lines:
    print(line)






