# -*- coding: utf-8 -*-
"""
Created on Thu Oct 18 12:01:15 2018

@author: azer
"""
from __future__ import division

import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

plt.clf;

ax = plt.gca();

# logarithmic y-axis
#ax.set_yscale('log');

#sort
green_array = np.sort(non_neg_vals_pad_01);
red_array = np.sort(non_neg_vals_pad_01_rigid);

green_array = [x for x in green_array if x != 0]
red_array = [x for x in red_array if x != 0]

#cumulative sub
green_cumsum = np.cumsum(green_array, dtype=float);
red_cumsum = np.cumsum(red_array, dtype=float);

# tyler
N = len(non_neg_vals_pad_01)
cumdensity = [(x + 1) / N for x in range(N)]

# tyler
N2 = len(non_neg_vals_pad_01_rigid)
cumdensity2 = [(x + 1) / N2 for x in range(N2)]

#print(N)
#print(len(cumdensity))
#print(len(non_neg_vals_pad_01))

# plot
#plt.plot(non_neg_vals_pad_01, cumdensity, 'g', linewidth=1);
#plt.plot(non_neg_vals_pad_01_rigid, cumdensity2, 'r', linewidth=1);

'''
norm1 = np.linalg.norm(pad_01_cumsum);
norm2 = np.linalg.norm(pad_01_rigid_cumsum);
pad_01_cumsum = pad_01_cumsum / norm1;
pad_01_rigid_cumsum = pad_01_rigid_cumsum/ norm2;
'''

plt.plot(green_array, green_cumsum, 'g');
plt.plot(red_array, red_cumsum, 'r');

#plt.plot([2.5865, 2.5865], [0, 400000], 'k-')

plt.title(r"Cumulative sum ");
plt.show();
