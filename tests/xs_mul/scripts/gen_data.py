#!/usr/bin/env python

import sys
import random

ROW=int(sys.argv[1])
COL=int(sys.argv[2])
STRIDE=int(sys.argv[3])
BYTES=int(sys.argv[4])
MAT=sys.argv[5]
PADS = STRIDE // BYTES - COL

if MAT == 'b':
    for row in range(ROW):
        for col in range(COL):
            data = random.randint(0, 255)
            print(f"{data:2d},", end='')
            #print(f"{1},", end='')
        for pad in range(PADS):
            print(f"{0:2d},", end='')
        print("")

if MAT == 'a':
    for row in range(ROW):
        for col in range(COL):
            data = random.randint(0, 255)
            print(f"{data:2d},", end='')
        for pad in range(PADS):
            print(f"{0:d},", end='')
        print("")

if MAT == 'c':
    for row in range(ROW):
        for col in range(COL):
            data = random.randint(0, 255)
            print(f"{data:3d},", end='')
            #print(f"{row * ROW + col:3d},", end='')
            #print(f"0,", end='')
        for pad in range(PADS):
            print(f"{0:3d},", end='')
        print("")
