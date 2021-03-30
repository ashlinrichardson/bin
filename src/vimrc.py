data = '''set number
set mouse=a
set tabstop=4
set shiftwidth=4
set softtabstop=4
set expandtab

if has('syntax')
  syntax on
endif

set autoindent

if has('mouse')
  set mouse=a
endif'''

import os
import sys
user = os.popen("whoami").read().strip()
homedir = os.popen("eval echo ~$" + user).read().strip()

f = homedir + os.path.sep + '.vimrc'

if not os.path.exists(f):
    open(f, 'wb').write(data.encode())
