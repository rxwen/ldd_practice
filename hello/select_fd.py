#!/usr/bin/env python
# -*- coding: utf-8 -*-

import select

# mknod heldev c MAJOR 0
f = open("heldev")

rl = [f]
wl = []
el = []

print "call select.select"
# select will block until some data has been written to heldev
rc = select.select(rl, wl, el)

print "select returned"
print rc

f.close()
