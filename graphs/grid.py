#!/usr/bin/python3
import random
import sys

if len(sys.argv) is not 4:
    print("Usage: %s number width heigth" % (sys.argv[0]))
    exit()

n = int(sys.argv[1])
x = int(sys.argv[2])
y = int(sys.argv[3])

print(n*n)

for i in range(0, n):
    for j in range(0, n):
        print(10 + i*x + random.randint(0,5), 10 + j*y + random.randint(0,5))
