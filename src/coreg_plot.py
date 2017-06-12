#!/usr/bin/env python
'''  This script for comparing different coregistration results
re: coreg_C4.m '''
import os
import sys
xs, ys, mi, si, cnt, sumX, sumY = {}, {}, {}, {}, {}, {}, {}

rsfolders = open("rs2folders.txt").readlines();
if len(rsfolders) != 11:
	print(len(rsfolders))
	print("Error: expected there to be 11 data sets");
	sys.exit(1)

nsets = len(rsfolders)

for line in open("coreg.txt").readlines():
	s = line.strip().split(",")
	print(s) #['step=2', 'masterI=11', 'i=10', '10.5', '6']
	miss = int((((s[1]).strip()).split("="))[1])
	pass
	iss =  int((((s[2]).strip()).split("="))[1])
	yss, xss =  float(s[3]), float(s[4])
	print miss, iss, yss, xss
	mi[miss], si[iss] = 1, 1
	index = [miss, iss]
	xs[str(index)] = xss
	ys[str(index)] = yss
	if(str(index) in cnt):
		cnt[str(index)] += 1
	else:
		cnt[str(index)] = 1
	if miss in sumX:
		sumX[miss] += abs(float(xss))
	else:
		sumX[miss] = abs(float(xss))
	if miss in sumY:
		sumY[miss] += abs(float(yss))
	else:
		sumY[miss] = abs(float(yss))

print xs, ys, "*", cnt, len(cnt)

for item in cnt:
	if(cnt[item]>1.):
		print("Error: greater than one instance of coregistration pair in log file.")
		print("Please delete duplicate entries and try again.")
		sys.exit(1)

print sumX, sumY 
min, mini = 0, 0
for i in range(1,nsets+1):
	total = sumX[i] + sumY[i]
	if i == 1:
		min = total
		mini = i;
	else:
		if total < min:
			min = total;
			mini = i;
	print "masterI=", i, "sum|X|=", sumX[i], "sum|Y|=", sumY[i], "sum|X|+sum|Y|=", total 

print "\n\nBest master: i=", mini, " with ", min, "\n\n"

for i in range(1, nsets + 1):
	folder = rsfolders[i - 1].strip()
	outfolder = "C2_" + str(i)
  outfolder = os.path.abspath(outfolder)
	if(os.path.exists(outfolder)):
		cmd = "rm "+outfolder
		a = os.system(cmd)
	infolder = folder + "/C2/C2_coreg/"
  infolder = os.path.abspath(infolder) 
	if i == mini: #for the master, use the "non-coregistered" folder (as the master is the target for coregistration, for the other data sets).
		infolder = folder + "/C2/"
	cmd = "ln -s  " + infolder + " " + outfolder #20130218/C2/C2_coreg/ C2_1"
	a = os.system(cmd)
	print a, cmd

