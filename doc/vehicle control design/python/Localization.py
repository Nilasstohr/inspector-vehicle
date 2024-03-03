import json

import numpy as np
from matplotlib import pyplot as plt
from numpy import loadtxt


class Localization:
    def __init__(self):
        xt = loadtxt("../../Measurements/localization/record2.txt")
        x = xt[:, 0]
        y = xt[:, 1]
        xm = np.array([40, 201, 119])
        ym = np.array([40, 53, 91])
        plt.scatter(xm, ym)
        plt.plot(x, y)
        plt.axis([0, 250, 0, 150])
        plt.grid()
        plt.ylabel('some numbers')
        plt.show()
