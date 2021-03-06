import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

# parameters
d_gap = 5.75;
h_step = 0.1; # gap
max_val = 20;
neg_space_ratio = 0.5; # negative space ratio 0..1.0

r_vals = np.arange(0.0, max_val, h_step );

'''
first_len = np.ceil(d_gap / h_step);
first_array = np.linspace(neg_space_ratio, 0, first_len);
second_array = np.zeros(len(r_vals) - len(first_array) );
tri_array = np.concatenate((first_array, second_array), axis = 0);
'''


tri_array = [];
for i in r_vals:
    tri_val = 0;
    if (i <= d_gap):
        tri_val = 1.0 - (i / d_gap);
        tri_val *= neg_space_ratio;
        #print 1.0 - (i / d_gap);
    tri_array.append(tri_val);


figfig = plt.figure(1);
figfig.patch.set_facecolor('white');
plt.clf();
ax = plt.gca();

plt.plot(r_vals, tri_array, 'r', linewidth=2);
plt.plot([d_gap, d_gap], [0, neg_space_ratio], 'k-')

plt.title(r"Test");
plt.show();

