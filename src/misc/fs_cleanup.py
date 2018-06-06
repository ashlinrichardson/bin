'''May 12, 2018-- helper script for restoring a file system
    - scan an fs
    - remove files with certain prefix
    - remove empty folders (this one should be recursive but isn't)
    - count files (keyed by extension)

might also need (before or after)
  sudo chmod -Rv 777 dir
  sudo chown -Rv username dir
  rsync -avz dir1 dir2
  rsync -avz --inplace dir1 dir2
'''
import os, sys; my_ext = {}

count_file = ".cleanup.py_count.txt"
past_count, count = None, 0
if os.path.exists(count_file):
	past_count = int(open(count_file).read().strip())

def s(x): # sort a dict on value
    import operator
    sorted_x = sorted(x.items(), key=operator.itemgetter(1), reverse=True)
    return sorted_x

def ext(f):
    fs = f.split(".")
    e = fs[-1] if (len(fs) >= 2) else ""
    e = e.strip()
    e = "" if e==None else e
    if e in my_ext:
    	my_ext[e] += 1
    else:
    	my_ext[e] = 1
    return e

# traverse root directory, and list directories as dirs and files as files
for root, dirs, files in os.walk("."):
    count += 1
    root = os.path.abspath(root)

    if os.path.isdir(root) and (os.listdir(root) == []):
        cmd = "rmdir " + root; print cmd
	try:
    	    os.rmdir(root)
 	except:
    	    print "Error:", cmd
	    sys.exit(1)

    for f in files:
	count += 1
	if (count % 100 == 0):
	    if past_count:
	        print 100. * float(count) / float(past_count)
 	    else:
		print "count", count

	e = ext(f)  # count by extension
        fn =  os.path.join(root, f) #pass #print(len(path) * '---', file)
	try:
	    if f[0:17]=="hfsprescue_dir_id":
	        cmd = "rm " + fn; print cmd
	        try:
	            os.remove(fn)
	        except:
		    print "Error:", cmd
		    sys.exit(1)
	except:
	    pass
print "done\n\ncount by file ext:\n\n"
print str(s(my_ext))

open(count_file,"wb").write(str(count))
sys.exit(0)

