#!/usr/bin/env python
import os
import sys

if((len(sys.argv) != 3) & (len(sys.argv) != 2)):
	print "ccf.py: forestry calculation: invert interpolated LHQ to solve for percentile height, which is used to evaluate interpolated CCF."
	print "Usage: ccf [input file] [input percentile height figure (float)]"
	print "Usage: ccf [input directory for files in *.txt format only] [input percentile height figure (float)"
	sys.exit(1)

if(len(sys.argv) == 3):
	filename = sys.argv[1]
	filename = os.path.abspath(filename)

	if(os.path.isfile(filename)):
		ph = float(sys.argv[2])
		f = open(filename)
		lines = f.readlines()
		nlines = len(lines) - 4
		f.close()
		os.system("ccfi " + filename + " " + str(nlines) + " " + str(ph))
	else:
		results = []
		j= 0 ;
		for my_file in os.listdir(filename):
			j=j+1
			print j, "of", len(os.listdir(filename))
			if(len(my_file.split(".txt")) == 1):
				continue;
			my_file = os.path.join(filename, my_file);
			print "FILE:",my_file
			ph = float(sys.argv[2])

	    f = open(my_file)
      lines = f.readlines()
      nlines = len(lines) - 4
			f.close()
			lines = os.popen("ccfi " + my_file + " " + str(nlines) + " " + str(ph)).readlines()

			for i in range(0,len(lines)):
				print lines[i].strip()

			ans = [str(my_file)]

			items = lines[len(lines) - 1].strip().split();
			for item in items:
				ans.append(item)
			results.append(ans)

		print "FILE PERCENTILE_HEIGHT LHQ_INTERP CCF_INTERP"
		for result in results:
			print (" ").join(result)
