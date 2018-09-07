# -*- coding: utf-8 -*-
"""
Created on Tue Jul 04 10:31:28 2017

@author: azer
"""

import numpy as np
import matplotlib.pylab as plt
import pandas as pd

import scipy.stats as stats

from scipy import signal

plt.figure(1)
plt.clf()

#####################
# normal distribution
#####################
plt.figure(1)
plt.clf()
mu, sigma = 0, 0.001 # mean and standard deviation
norm_func = np.random.normal(mu, sigma, 10000)
count, bins, ignored = plt.hist(norm_func, 100, normed=True)
plt.plot(bins, 1/(sigma * np.sqrt(2 * np.pi)) *
         np.exp( - (bins - mu)**2 / (2 * sigma**2) ),
        linewidth=2, color='r')
plt.show()
kurt_n = stats.kurtosis(norm_func, fisher=False, bias=False);
skew_n = stats.skew(norm_func, bias=False);