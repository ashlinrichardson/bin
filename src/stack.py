#!/usr/bin/env python2.7
'''
  stack.py

  takes a series of single-band images and "cat"s them together into a BSQ format image.

- the single-band images are checked for consistency
- header fields are preserved, where possible
- need to read and write headers in C++ and python

'''

import os
import sys

print "usage: stack [input data frame (path)] [output path (empty folder)]"

