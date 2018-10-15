import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

r_vals = np.arange(0.2, 21, 0.5);

# cuz area fraction
#r_vals = np.insert(r_vals, 0, 0)
#r_vals.insert(0, 0);


#what is d_gap
# 2.5865



"""
fig1 = plt.figure(1);
fig1.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

plt.plot(r_vals, np.asarray(c_vals_cf_bad), 'r');
#plt.plot(r_vals, np.asarray(c_vals_1), 'b');

plt.title(r"Cov Bad");
plt.show();

fig2 = plt.figure(2);
fig2.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

plt.plot(r_vals, np.asarray(c_vals_cf_good), 'b');
#plt.plot(r_vals, np.asarray(c_vals_1), 'b');

plt.title(r"Cov Good");
plt.show();
"""

fig3 = plt.figure(3);
fig3.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

# logarithmic y-axis
#ax.set_yscale('log')

#plt.plot(r_vals, np.asarray(scf_vals_balabolka_01), '#2FADF4', linewidth=1);
#plt.plot(r_vals, np.asarray(scf_vals_balabolka_man2x), '#ED2B2B', linewidth=1);
#plt.plot(np.asarray(scf_vals_balabolka_01), '#2FADF4', linewidth=1);
#plt.plot(np.asarray(scf_vals_balabolka_man2x),  '#ED2B2B', linewidth=1);


#line_up, = plt.plot([1,2,3], label='Line 2')
#line_down, = plt.plot([3,2,1], label='Line 1')
#plt.legend(handles=[line_up, line_down])\

plt.hist(np.asarray(non_neg_vals_boxes_a), bins = r_vals, color='r', alpha = 0.5, histtype='step', normed=True);
plt.hist(np.asarray(non_neg_vals_boxes_c), bins = r_vals, color='g', alpha = 0.5, histtype='step', normed=True);
plt.hist(np.asarray(non_neg_vals_boxes_e), bins = r_vals, color='b', alpha = 0.5, histtype='step', normed=True);

plt.plot([8.0975, 8.0975], [0, 0.15], 'k-')



plt.title(r"boxes ace rgb");
plt.show();