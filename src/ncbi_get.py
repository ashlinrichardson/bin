'''retrieve gene sequence / length etc from NCBI nuccore db,
using linux/unix command esearch. Usage:
    python3 ncbi_get.py [accension ID for gene]
deps:
    sudo apt install ncbi-entrez-direct'''
import os
import sys
args = sys.argv
sep, exists = os.path.sep, os.path.exists

def err(m):
    print('Error:' + str(m)); sys.exit(1)

if not exists('dat'):
    os.mkdir('dat')

result = os.popen("/usr/bin/esearch 2>&1").read().strip()
if result != 'Must supply -db database on command line':
    err('please run: \nsudo apt install ncbi-entrez-direct')

accension = args[1] if len(args) > 1 else 'NM_001162936'
o_f = 'dat' + sep + accension + '.gb'
if not exists(o_f):
    d = os.popen('esearch -db nuccore -query "' +
                 accension +
                 '" | efetch -format gb').read()
    # print('+w', o_f)
    open(o_f, 'wb').write(d.encode())
x = open(o_f, 'rb').readline()  # seq length lives in the first line
w = [x.decode() for x in x.strip().split()] # print(w)
# ['LOCUS', 'NM_001162936', '1016', 'bp', 'mRNA', 'linear', 'PRI', '20-DEC-2021']
if w[3] != 'bp':
    err('unexpected string')
print(int(w[2]))  # output gene seq length (using accension ID e.g. NM_001162936)
