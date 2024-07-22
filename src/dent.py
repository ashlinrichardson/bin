#!/usr/bin/env python
import os
import sys
from misc import normpath, wopen, error, run, chkfile, args, exists, ext
from ansicolor import KRED, KGRN, KNRM
''' indentation for c/c++ code, by ashlin richardson 20170617'''

if len(args) < 2:
    error('dent.py: format C/C++ files for indentation (2 spaces per tab)' +
          '\n\tusage: dent [filename]')

in_file = normpath(args[1])

# there's a python version of this program, too:
if ext(in_file) == 'py':
    error('python not supported yet')

    # for python, run that one instead
    run('pydent ' + in_file)
    sys.exit(0)

# default for spaces-per-tab, 2
spaces_per_tab = 2

if len(args) > 2:
    # if there's an extra arg, try to interpret it as "spaces per tab"
    try:
        spaces_per_tab = int(args[2])
    except:
        error('could not parse int')

# check the input file is there
chkfile(in_file)
ext = in_file.split('.')[-1]
if not (ext in ['h', 'c', 'cpp', 'js', 'R']):
    error('only c or cpp files supported')

n_indent = 0  # indentation level
indent = ' ' * spaces_per_tab
dat = open(in_file).read()

bak_file = in_file + '.bak'
wopen(bak_file).write(dat.encode())

# after making the backup:
dat = dat.replace('}else{', '}\nelse{')
dat = dat.replace(', {', ',{')
dat = dat.replace('},{', '},\n{')
# dat = dat.replace('{', '{\n')
# dat = dat.replace('\n\n', '\n')
new_lines, lines = [], dat.strip().split('\n')

for i in range(0, len(lines)):
    line = lines[i].strip()

    # if line == '':
    #    continue

    line = ' '.join(line.split())
    reindent = (n_indent * indent) + line

    # red for lines that changed, green for unchanged
    if(reindent != lines[i].rstrip()):
        sys.stdout.write(KRED)
    else:
        sys.stdout.write(KGRN)

    # check if brackets are changing indentation level
    last_char = None
    last_chars = None
    try:
        last_chars = line[-2:]
    except:
        pass
    try:
        last_char = line[-1]
    except:
        pass

    if last_char == '{' or last_char == '[':
        n_indent += 1
    elif last_char == '}' or last_char == ']' or last_chars == '};' or last_chars == '},' or last_chars == '],' or last_chars == '];':
        n_indent -= 1
        reindent = (n_indent * indent) + line

    print(reindent)

    # the new lines to be written
    if reindent.strip() != '':
        new_lines.append(reindent)
    else:
        new_lines.append('')

if(n_indent != 0):
    print("n_indent", n_indent, "algorithm may have missed a:")
    print("opening bracket" if n_indent < 0 else "closing bracket")
    print('indentation level not 0: either there are open brackets, ' +
          ' or the logic of this program is too simple')

# add a few filters
new_lines_filt = []

for i in range(0, len(new_lines)):
    if i > 0 and new_lines[i].strip() == '{' and new_lines_filt[-1][-2:] != '},' and new_lines_filt[-1][-1] not in ['{', '}', '[', ']']:
        new_lines_filt[-1] += '{'
    else:
        new_lines_filt.append(new_lines[i])

out = '\n'.join(new_lines_filt).replace('\t', spaces_per_tab * ' ')
out = out.replace('\n\n\n', '\n\n')
wopen(in_file).write(out.encode())
sys.stdout.write(KNRM)
