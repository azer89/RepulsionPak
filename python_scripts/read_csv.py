# -*- coding: utf-8 -*-
"""
Created on Sun Jul 02 10:23:30 2017

@author: azer
"""

import numpy as np
import matplotlib.pylab as plt
import pandas as pd

import scipy.stats as stats

from scipy import signal



#####################
# normal distribution
#####################
plt.figure(1)
plt.clf()
mu, sigma = 0, 0.1 # mean and standard deviation
norm_func = np.random.normal(mu, sigma, 1000)
count, bins, ignored = plt.hist(norm_func, 30, normed=True)
plt.plot(bins, 1/(sigma * np.sqrt(2 * np.pi)) *
         np.exp( - (bins - mu)**2 / (2 * sigma**2) ),
        linewidth=2, color='r')
plt.show()
kurt_n = stats.kurtosis(norm_func, fisher=False, bias=False);
skew_n = stats.skew(norm_func, bias=False);

#######################
# gaussian distribution
#######################
plt.figure(2)
plt.clf()
g_func = signal.gaussian(100, std=50)
mean_g = np.mean(g_func)
#g_func -= g_mean
plt.plot(g_func)
plt.title(r"Gaussian window")
plt.ylabel("Amplitude")
plt.xlabel("Sample")
kurt_g = stats.kurtosis(g_func, fisher=False, bias=False);
skew_g = stats.skew(g_func, bias=False);

#######################
# CSV
#######################
csv_data=pd.read_csv('C://Users//azer//workspace//Images//PhysicsPak_Snapshots_01//dist_norm.csv', sep=',',header=None)
#C://Users//azer//OneDrive//__results__//histogram_thing//01_good_cat_02//dist_norm_ggod_cat_02.csv
#csv_data=pd.read_csv('C://Users//azer//OneDrive//__results__//histogram_thing//01_good_cat_01//dist_norm.csv', sep=',',header=None)
#C://Users//azer//OneDrive//__results__//histogram_thing//01_bad_cat_03
norm_df = csv_data.values[:,0]
#plt.figure(2)
#plt.clf()
#plt.plot(norm_df)
#plt.title(r"DF")
mean_df = np.mean(norm_df)
kurt_df = stats.kurtosis(norm_df, fisher=False, bias=False);
skew_df = stats.skew(norm_df, bias=False);

"""
If I use gaussian func with small stdev (really tall peak) the kurt is really high
statistical bias?
"""

