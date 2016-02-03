#!/usr/bin/python3
import random
import sys

n = int(sys.argv[1])
x = int(sys.argv[2])
y = int(sys.argv[3])

print(n)

for i in range(0, n):
    print(random.randint(10,x), random.randint(10,y))
