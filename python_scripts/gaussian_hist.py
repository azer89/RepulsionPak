# -*- coding: utf-8 -*-
"""
Created on Tue Jul 04 09:44:49 2017

@author: azer
"""

import numpy as np
import matplotlib.pylab as plt
import pandas as pd

import scipy.stats as stats

from scipy import signal

plt.figure(1)
plt.clf()

sigma = 1500
g_func = signal.gaussian(10000, std=sigma)
mu = np.mean(g_func)

count, bins, ignored = plt.hist(g_func, 50, normed=True)

plt.plot(bins, 1/(sigma * np.sqrt(2 * np.pi)) *
         np.exp( - (bins - mu)**2 / (2 * sigma**2) ),
        linewidth=2, color='r')
plt.show()

kurt_g = stats.kurtosis(g_func, fisher=False, bias=False);
skew_g = stats.skew(g_func, bias=False);

