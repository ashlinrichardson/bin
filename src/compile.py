#!/usr/bin/python2.7
'''compile scripts: this script expects to live at relative path:
      src/compile/compile.py'''
import os
import re
import sys
import copy
import time
from fl0w3r import error, exists, normpath, normdir

opt_flags, t, args = '-O3', time.localtime(), sys.argv
bin_folder, source_folder = '', ''

if os.name != "posix":
    error('defined for posix')

head, tail = os.path.split(normdir(normpath(args[0])))
# head, tail = os.path.split(head)

if tail == 'bin':
    error('please run from src/ folder')
else:
    source_folder = head
    if tail != 'compile':
        error('expected to reside within a directory compile/')
    head, tail = os.path.split(head)
    if tail != 'src':
        error('expected compile/compile.py to reside within a directory src/');
    if os.path.basename(__file__) != 'compile.py':
        error('expected __file__ to be compile.py')   
    bin_folder = head + '/bin/'

if bin_folder == '':
    error('bin/ folder undefined')

if source_folder == '':
    error('src/ folder undefined')

if not exists(source_folder):
    error('folder not found: ' + source_folder)

if not exists(bin_folder):
    error('folder not found: ' + bin_folder)

if not os.path.isdir(bin_folder):
    error('not a directory: ' + bin_folder)

if len(sys.argv) < 2:
    error('compile.py re-implemented 20170602 by ashlin richardson\n\tUsage: python src/compile.py src/name_of_program_to_compile')
error('exit1')

bin_folder = normpath(bin_folder)
f = os.path.abspath(args[1].strip())
cmdname = f.split("/")
cmdname = cmdname[len(cmdname) - 1]
fileend = copy.deepcopy(cmdname)
ext = fileend.split(".")
ext = ext[len(ext) - 1]
compiler_cmd = ''
if ext == 'cpp':
    compiler_cmd = 'g++ -w ' + opt_flags + ' -lm '
elif ext == 'c':
    compiler_cmd = 'gcc -w ' + opt_flags + ' -lm '
elif ext == 'py':
    pass
else:
    error('file extension not supported')

cmdname =  fileend.split('.')
cmdname = cmdname[0]
binname = bin_folder + cmdname
cmd = 'rm -rf ' + binname
print cmd
a = os.system(cmd)
command_args = ""
time_string =( "%04d" % t.tm_year)+("%02d" % t.tm_mon)+("%02d" % t.tm_mday)+("%02d" % t.tm_hour)+("%02d" % t.tm_min)+("%02d" % t.tm_sec);

if ext == 'cpp':
    command_args = compiler_cmd + " "+f+ " -o "+binname+".exe"
    a = os.system(command_args)
    binfile = open(binname, "w");
    binfile.write("#!/usr/bin/python\nimport sys,os\nargs=sys.argv;\nargs=args[1:];\nargs=(' ').join(args);\nprint('last compiled"+str(t)+"');\na=os.system('"+binname+".exe ' +args);\n");
    binfile.close();
elif ext == 'c':
    command_args = compiler_cmd + " "+f+ " -o "+binname+".exe"
    a=os.system(command_args)
    binfile = open(binname, "w");
    binfile.write("#!/usr/bin/python\nimport sys,os\nargs=sys.argv;\nargs=args[1:];\nargs=(' ').join(args);\nprint('"+str(t)+" last compiled');\na=os.system('"+binname+".exe ' +args);\n");
    binfile.close();
elif ext == 'py':
    srcfile = open(f)
    src_contents = srcfile.read();
    srcfile.close()
    binfile = open((binname),'w')
    binfile.write( "#!/usr/bin/python\nprint('"+time_string+" last compiled');\n"+src_contents)
    binfile.close()
else:
    error('extension not supported')

print command_args
f = open(bin_folder + '.' + cmdname + '.README', 'w')
f.write('compile.py\n')
f.write('time: ' + time_string + '\n');
f.write('binary directory: ' + bin_folder + '\n')
f.write('command arguments used: ' + command_args + '\n')
f.close()

if(os.path.exists(binname)):
    os.system('chmod 755 ' + binname)
    sys.exit(0)
else:
    error('Error: compilation failed')
