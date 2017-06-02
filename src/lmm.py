#!/usr/bin/env python
'''want to know where your disk space is used? 
summarize file space usage, producing a grand total (du -c), with sorting
'''
import os, sys
def msg():
  print("lmm.py\n\tlmm.py\t\t\t\t(this defaults to current directory)\n\tlmm.py [path]\t\t\t\t(this allows a path to be specified.\n"); 
  sys.exit(1);
args = sys.argv
pwd = os.path.abspath( os.popen("pwd").read().strip() );
if(len(args)>1):
  pwd = os.path.abspath( args[1] );
  if( not( os.path.exists( pwd)) or not( os.path.isdir(pwd))):
    msg();
cmd = "du -c "+pwd
a =os.popen(cmd).read().strip().split("\n");
lines = [ ] 
for line in a:
  #print line.strip();
  ls = line.strip().split();
  n = int(ls[0]);
  rest = (" ".join(ls[-1:])).strip();
  lines.append(  [n, line]);#rest] )
lines.sort(key=lambda x: x[0])

for line in lines:
  print(str(line[0])+" "+line[1]);

