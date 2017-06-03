#!/usr/bin/env python2.7
'''compile scripts: this script expects to live at relative path:
      src/compile.py'''
import os
import re
import sys
import copy
import time
from ansicolor import KYEL, KNRM
from fl0w3r import error, exists, normpath, normdir, timestring, run, printw

time_string = timestring()
opt_flags, args = '-O3', sys.argv
bin_folder, source_folder = '', ''

if os.name != "posix":
    error('defined for posix')

head, tail = os.path.split(normdir(normpath(args[0])))
source_folder = normpath(head)
head, tail = os.path.split(head)

if tail == 'bin':
    error('please run from src/ folder')
else:
    if tail != 'src':
        error('expected compile.py to reside within a directory src/')
    if os.path.basename(__file__) != 'compile.py':
        error('expected __file__ to be compile.py')
    bin_folder = normpath(head + '/bin/')
    if not exists(bin_folder):
        run('mkdir -p ' + bin_folder)
    bin_folder = normpath(bin_folder)
    if not exists(bin_folder):
        error('failed to create folder: ' + bin_folder)

if not exists(source_folder):
    error('folder not found: ' + source_folder)

if not os.path.isdir(bin_folder):
    error('not a directory: ' + bin_folder)

if len(sys.argv) < 2:
    msg = 'compile.py re-implemented 20170602 by ashlin richardson\n'
    msg += '\tUsage: python src/compile.py src/name_of_program_to_compile'
    error(msg)

f = os.path.abspath(args[1].strip())
cmdname = f.split("/")[-1]

fileend = copy.deepcopy(cmdname)
ext = fileend.split(".")[-1]

compiler_cmd = ''
if ext == 'cpp':
    compiler_cmd = 'g++ -w ' + opt_flags + ' -lm '
elif ext == 'c':
    compiler_cmd = 'gcc -w ' + opt_flags + ' -lm '
elif ext == 'py':
    pass
else:
    error('file extension not supported')

cmdname = fileend.split('.')[0]
binname = normpath(normpath(bin_folder) + cmdname)

if exists(binname):
    cmd = 'rm -rf ' + binname
    print cmd
    a = os.system(cmd)

command_args, dat = '', None
shebang = "#!/usr/bin/python\n"
last_comp = "print('" + comp_m + "')\n"
comp_m = "compile time: " + str(time_string)
imp = "import sys\nsys.path.append('" + source_folder + "')\n"
command_args = compiler_cmd + ' ' + f + ' -o ' + binname + '.exe'

dat = shebang + "import os\n" + imp
dat += "args = sys.argv[1:]\nargs = (' ').join(args)\n"
dat += last_comp + "a = os.system('"+binname+".exe ' + args)\n"

if ext == 'cpp' or ext == 'c':
    print KYEL + command_args + KNRM
    a = os.system(command_args)
    open(binname, 'w').write(dat)
elif ext == 'py':
    src_contents = open(f).read()
    dat = shebang + imp + last_comp + src_contents
    open((binname), 'w').write(dat)
else:
    error('extension not supported')
printw(binname)

readme_fn = bin_folder + '.' + cmdname + '.README'
wdat = 'compile.py\n' + comp_m + '\nbinary directory: ' + bin_folder
open(readme_fn, 'w').write(wdat + '\ncommand arguments used: ' + command_args)

if(os.path.exists(binname)):
    os.system('chmod 755 ' + binname)
    sys.exit(0)
else:
    error('Error: compilation failed')
