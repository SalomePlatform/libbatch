#!/usr/bin/env python

import sys
import os

if len(sys.argv) != 4:
  print "Usage: test-script.py seta.py setb.py result.txt"

execfile(sys.argv[1])
execfile(sys.argv[2])

c = a * b

f = open(sys.argv[3], "w")
f.write('MYENVVAR = "%s"\n' % os.getenv("MYENVVAR"))
f.write("c = %d\n" % c)
