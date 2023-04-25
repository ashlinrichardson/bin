'''20230425 trim N characters from the left of each line of the text input file
'''
import os
import sys
args = sys.argv

def err(m):
    print("Error:", m); sys.exit(1)

if len(args) < 3:
    err('python3 ltrim.py [input text file] [number of characters to trim from the left]')

f, n = args[1], args[2]

lines = [x.rstrip() for x in open(f).readlines()]

o_f = f + '_ltrim.txt'
print('+w', o_f)
open(o_f, 'wb').write(('\n'.join(([x[n:] for x in lines]))).encode())
