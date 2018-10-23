import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

r_vals = np.arange(0.2, 21, 0.2);

# cuz area fraction
#r_vals = np.insert(r_vals, 0, 0)

fig3 = plt.figure(3);
fig3.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

green_array = non_neg_vals_pad_01;
red_array =   non_neg_vals_pad_man2x;


plt.hist(red_array, bins = r_vals, color='r', histtype='step', normed=True);
plt.hist(green_array, bins = r_vals, color='g', histtype='step', normed=True);

# logarithmic y-axis
#plt.yscale('log');
#plt.gca().minorticks_off();

plt.title(r"Artist (red) vs RepulsionPak (green)");
plt.show();