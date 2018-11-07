import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

# input arrays
green_array = np.asarray(scf_vals_boxes_a);
red_array   = np.asarray(scf_vals_boxes_c);

# parameters
d_gap = 8.0975;
h_step = 0.1; # gap
max_val = 20;
neg_space_ratio = area_fraction; # negative space ratio 0..1.0

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
    
# arrays
green_array_dif = green_array - tri_array;
red_array_dif = red_array - tri_array;

green_array_dif = [x for x in green_array_dif if x != 0];
red_array_dif = [x for x in red_array_dif if x != 0];

green_idx = r_vals[:len(green_array_dif)];
red_idx = r_vals[:len(red_array_dif)];


figfig = plt.figure(1);
figfig.patch.set_facecolor('white');
plt.clf();
ax = plt.gca();

plt.plot(r_vals, tri_array, '#000000', linewidth=2);

plt.plot(green_idx, green_array_dif, 'g', linewidth=2);
plt.plot(red_idx, red_array_dif, 'r', linewidth=2);

plt.plot([d_gap, d_gap], [0, 0.04], 'k-')

plt.title(r"Test");
plt.show();

