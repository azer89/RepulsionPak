# -*- coding: utf-8 -*-
"""
Created on Thu Jul 06 10:48:39 2017

@author: azer
"""

import numpy as np
import matplotlib.pylab as plt
import pandas as pd

import scipy.stats as stats
#from scipy.stats import iqr

from scipy import signal

#######################
# CSV
#######################
#filename = 'C://Users//azer//OneDrive//__results__//03_animal_good_02//PhysicsPak_Snapshots_01//dist.csv';
#filename = 'C://Users//azer//OneDrive//__results__//03_circle_bad//PhysicsPak_Snapshots_01//dist.csv';
filename = 'C://Users//azer//workspace//Images//PhysicsPak_Snapshots_01//dist_all.csv';
#filename = 'C://Users//azer//OneDrive//__results__//03_unilever2//PhysicsPak_Snapshots_01//dist.csv';
csv_data=pd.read_csv(filename, sep=',',header=None);
print filename;
df = csv_data.values[:,0]
mean_df = np.mean(df);
df_min_mean = np.abs(df - mean_df);
#df_log10 = np.log10(df);

# uhhhh doesnt work ???
#mode_df = stats.mode(df);

#######################
# HISTOGRAM 1
#######################
fig1 = plt.figure(1);
fig1.patch.set_facecolor('white')
plt.clf();
max_df = np.max(df);
min_df = np.min(df);
#num_bin = max_df  / 0.1
#bin_size = (max_df - min_df) / 0.1;
#bin_size = int(bin_size);
bin_size = 100;
plt.hist(df, bins = bin_size, histtype='stepfilled', color='#398bcb', range=(0, 20));
plt.title(r"Histogram");
print "bin size = ", bin_size;

#######################
# HISTOGRAM 2
#######################
fig2 = plt.figure(2);
fig2.patch.set_facecolor('white')
plt.clf();
#num_bin = max_df  / 0.1
plt.hist(df, bins = bin_size, histtype='stepfilled', cumulative=True, color='#398bcb',  range=(0, 20));
plt.title(r"Cumulative histogram");

#######################
# HISTOGRAM 3
#######################
fig3 = plt.figure(3);
fig3.patch.set_facecolor('white')
plt.clf();
#max_df = np.max(df)
#num_bin = max_df  / 0.1
plt.hist(df_min_mean, bins = bin_size, histtype='stepfilled',  color='#398bcb',  range=(0, 20));
plt.title("|| x - mean || histogram");


#mean_df = np.mean(df);
median_df = np.median(df);
stdev_df = np.std(df);
kurt_df = stats.kurtosis(df, fisher=False, bias=False);
skew_df = stats.skew(df, bias=False);

q75, q25 = np.percentile(df, [75 ,25])
iqr_df = q75 - q25
#iqr_df = iqr(df);

###
'''
num_std_1 = 0.0;
num_std_2 = 0.0;  
num_std_3 = 0.0; 
mean_std_1 = mean_df + stdev_df;
mean_std_2 = mean_df + stdev_df * 2.0;
mean_std_3 = mean_df + stdev_df * 3.0;
for a in range(len(df)):
    if(df[a] < mean_std_1):
        num_std_1 += 1.0;
    if(df[a] < mean_std_2):
        num_std_2 += 1.0;
    if(df[a] < mean_std_3):
        num_std_3 += 1.0;
print "ratio mean + std * 1 is ", (num_std_1 / len(df));
print "ratio mean + std * 2 is ", (num_std_2 / len(df));
print "ratio mean + std * 3 is ", (num_std_3 / len(df));
'''

### SORT
'''
df_sort = np.sort(df);
idx_ten = len(df_sort) * 10 / 100;
idx_fifty = len(df_sort) * 50 / 100;
idx_ninety = len(df_sort) * 90 / 100;
span_val = (df_sort[idx_ninety] - df_sort[idx_ten]) / df_sort[idx_fifty];
print "d10 is ", df_sort[idx_ten]
print "d50 is ", df_sort[idx_fifty]
print "d90 is ", df_sort[idx_ninety]
print "mean is ", mean_df
print "max is ", max_df
print "span is ", span_val
'''    
print "mean   =", mean_df;
print "median =", median_df;
print "stdev  =", stdev_df;
print "skew   =", skew_df;
print "kurt   =", kurt_df;
print "iqr    =", iqr_df;
