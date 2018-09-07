# -*- coding: utf-8 -*-
"""
Created on Thu Jul 06 10:48:39 2017

@author: azer
"""

import numpy as np
import matplotlib.pylab as plt
import pandas as pd

import scipy.stats as stats

from scipy import signal

#######################
# CSV
#######################
csv_data=pd.read_csv('C://Users//azer//workspace//Images//PhysicsPak_Snapshots_01//dist_norm.csv', sep=',',header=None)
#C://Users//azer//OneDrive//__results__//histogram_thing//01_good_cat_02//dist_norm_ggod_cat_02.csv
#csv_data=pd.read_csv('C://Users//azer//OneDrive//__results__//histogram_thing//01_good_cat_01//dist_norm.csv', sep=',',header=None)
#C://Users//azer//OneDrive//__results__//histogram_thing//01_bad_cat_03
norm_df = csv_data.values[:,0]
plt.figure(2)
plt.clf()
plt.hist(norm_df, 100)
#plt.plot(norm_df)
plt.title(r"DF")
mean_df = np.mean(norm_df)
kurt_df = stats.kurtosis(norm_df, fisher=False, bias=False);
skew_df = stats.skew(norm_df, bias=False);
