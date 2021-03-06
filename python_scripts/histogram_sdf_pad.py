import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

r_vals = np.arange(0.2, 21, 0.2);

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

#plt.hist(np.asarray(scf_vals_balabolka_01), 10, normed=True);

plt.hist(np.asarray(non_neg_vals_pad_01_jitter), bins = r_vals, color='r', alpha = 0.5, histtype='step');

plt.hist(np.asarray(non_neg_vals_pad_01), bins = r_vals, color='g', alpha = 0.5, histtype='step');

plt.plot([5.75, 5.75], [0, 0.2], 'k-')


plt.title(r"Compare PAD");
plt.show();