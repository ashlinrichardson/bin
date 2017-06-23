#!/usr/bin/env python
'''textual analysis, unspecified program fragment '''
import os
import sys
import math
import string
import matplotlib

def deriv(x):
  y = []
  for i in range(1,len(x)):
    y.append( x[i]-x[i-1] )
  return(y)

fn, f = None, None
try: 
    fn = sys.argv[1]
    f = open(fn).read().strip()
except:
    print("hhist.py:\tusage:\n\thhist.py [input text file name]")
    sys.exit(1)

word_count = { } 
w = f.split()
for i in range(0,len(w)):
  wi = w[i]
  for p in string.punctuation:
    wi = wi.strip(p)
  wi = wi.lower()
  if(wi in word_count):
    word_count[wi] += 1
  else:
    word_count[wi] = 1

words = sorted(word_count.items(), key=lambda x: x[1], reverse=True)
t, x = [], [] 
total  = 0  
ci = 0 
for w in words:
  total += w[1]
  t.append(ci); ci+= 1 
  x.append(w[1]) 

n = 111  
import matplotlib.pyplot as plt
plt.plot( t[1:n], deriv( x[0:n]))
plt.tight_layout()
plt.savefig('wc.png')

def sgn(x):
  if( x < 0.):
    return -1.
  else:
    return 1.

def sign_change(x):
  ch = [0] 
  s = sgn(x[0])
  for i in range(1,len(x)):
    si = sgn(x[i])
    if(s != si):
      ch.append( i )
      s = si
  return ch


ch = sign_change(deriv(x))
i = 0
of = open('wc.dat', 'wb')
c0 = ch[1]
words0 = words[0:c0]
w0w = [] 
for w in words0:
  w0w.append( w[0])

for c in ch:
  of.write(str(c))
  of.write(str(words[i:c]))
  of.write('\n')
  i = c

of.close()

word_count_t = { }
w = f.split()
word_seq = [ ] 
word_t = [ ] 
for i in range(0,len(w)):
  wi = w[i]
  for p in string.punctuation:
    wi = wi.strip(p)
  wi = wi.lower()
  word_seq.append(wi.replace(',',''))

for i in range(1,len(word_seq)):
  word_t.append(str(word_seq[i-1])+','+str(word_seq[i]))

def wordcount(w):
  pass
  #need to write this... 

for w in word_t:
  if w in word_count_t:
    word_count_t[w] += 1
  else:
    word_count_t[w] = 1

words_t = sorted(word_count_t.items(), key=lambda x: x[1], reverse=True)

of = open('wc_t_geq1.dat','wb')
og = open('wc_t_geq1_ntin_w0.dat', 'wb')

for i in range(0,len(words_t)):
  if( words_t[i][1] > 1):
    w1, w2 = words_t[i][0].split(',')
    ds = str(words_t[i][1]) + ' : ' + str( words_t[i][0]) +'\n'
    of.write(ds)
    if((w1 in w0w) and (w2 in w0w)):
      print "w1", w1, "w2", w2
    else:
      og.write(ds)
of.close()
og.close()
print ""
print w0w

# extend to TRIPLES.... (n-tuples).... 

# "normal" analysis AB = BA on buckets.. 

