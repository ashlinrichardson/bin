'''auto-indentation for python programs (hope it works) 20170622
we ran this program on itself: no change to the result
'''
import os
import sys
from copy import deepcopy
from fl0w3r import args, ext, stem, leaf, error, normpath, wopen, chkfile

msg = 'pydent.py:\n\tusage: pydent.py [filename.py]'

if len(args) < 2:
    error(msg)

fn = normpath(args[1])
if ext(fn) != 'py':
    error(msg + '. only .py files accepted')

chkfile(fn)

# create a backup of the input file (to be overwritten)
bf = fn + '.bak'
wopen(bf).write(open(fn).read())

# indentation level
n_ind = 0

# number of spaces in front of last line
spaces_last = None

spaces_per_tab = 4
indent, dat = ' ' * spaces_per_tab, []

lines = open(fn).read().strip().split('\n')
for i in range(0, len(lines)):
    line = lines[i].strip()
    # print (n_ind * indent) + line
   
    if line == '':
        dat.append('')
        continue  
    
    lin = deepcopy(lines[i]).replace('/t', indent)
    n_spaces = len(lin) - len(lin.lstrip(' '))
    
    # decrease indent, if the indent decreases
    if not spaces_last is None:
        if n_spaces < spaces_last:
            n_ind -= 1  # decrease indent

    spaces_last = n_spaces

    dat.append(((n_ind * indent) + line).rstrip())

    # increase indent, if line ends in ':'
    if line[-1] == ':':
        n_ind += 1  # increase indent

wopen(fn).write('\n'.join(dat))
