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
non_neg_vals_pad_01 = np.sort(non_neg_vals_balabolka_01);
non_neg_vals_pad_01_rigid = np.sort(non_neg_vals_balabolka_man2x);

non_neg_vals_pad_01 = [x for x in non_neg_vals_pad_01 if x != 0]
non_neg_vals_pad_01_rigid = [x for x in non_neg_vals_pad_01_rigid if x != 0]

#cumulative sub
pad_01_cumsum = np.cumsum(non_neg_vals_pad_01, dtype=float);
pad_01_rigid_cumsum = np.cumsum(non_neg_vals_pad_01_rigid, dtype=float);

N = len(non_neg_vals_pad_01)
cumdensity = [(x + 1) / N for x in range(N)]

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

plt.plot(non_neg_vals_pad_01, pad_01_cumsum, 'g');
plt.plot(non_neg_vals_pad_01_rigid, pad_01_rigid_cumsum, 'r');

#plt.plot([2.5865, 2.5865], [0, 400000], 'k-')

plt.title(r"Compare ");
plt.show();
