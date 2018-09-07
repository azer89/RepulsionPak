# -*- coding: utf-8 -*-
"""
Created on Tue Jul 04 11:35:39 2017

@author: azer
"""

import numpy as np
import matplotlib.pylab as plt
import pandas as pd

import scipy.stats as stats

from scipy import signal

#from random import randint
#print(randint(0, 9))

plt.figure(1)
plt.clf()

dumdum = np.zeros(10000)
dumdum.fill(100)

#for i in range(0, len(dumdum)):
#    dumdum[i] += randint(0, 1)

plt.hist(dumdum, 10, normed=True)

kurt_sb = stats.kurtosis(dumdum, fisher=False, bias=False);
skew_sb = stats.skew(dumdum, bias=False);