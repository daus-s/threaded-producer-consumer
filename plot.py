#!/usr/bin/env python
# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import numpy as np

def main(argv):
    f = open("data.txt", "r")
    l = f.readline()
    vs = []
    while l:
        l = f.readline()
        r = l.split(":")
        #print(r)
        if len(r)==2:
            r[1] = r[1].strip()
            vs.append(float(r[1]))
    f.close()

    t = np.arange(1, 1365)

    f = plt.figure(figsize=(8,6))
    a = plt.axes()
    a.plot(t, vs)
    a.set_xlabel("number of items")
    a.set_ylabel("success rate")



if __name__ == '__main__':
    import sys
    main(sys.argv)
