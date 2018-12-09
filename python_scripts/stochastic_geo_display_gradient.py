import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

h_gap = 0.2;
r_vals = np.arange(h_gap, 20, h_gap);
r_vals = np.insert(r_vals, 0, 0);


fig3 = plt.figure(1);
fig3.patch.set_facecolor('white');
plt.clf();
ax = plt.gca();

green_array = scp_vals_boxes_layered_a;
red_array   = scp_vals_boxes_layered_c;

#blue_array = [x for x in blue_array if x != 0]
green_array = [x for x in green_array if x != 0];
red_array = [x for x in red_array if x != 0];

green_idx = r_vals[:len(green_array)];
red_idx = r_vals[:len(red_array)];

red_array_grad = np.gradient(red_array);
green_array_grad = np.gradient(green_array);

#red_array_grad = np.insert(red_array_grad, 0, 0, axis=0);
#green_array_grad = np.insert(green_array_grad, 0, 0, axis=0);

plt.plot(red_idx, np.asarray(red_array_grad), 'r', linewidth=1);
plt.plot(green_idx, np.asarray(green_array_grad), 'g', linewidth=1);
# balabolka 2.586
# pad 5.746
# box 8.276
plt.plot([8.276, 8.276], [0, -0.005], 'k--')

#plt.plot(r_vals, np.asarray(scf_vals_boxes_e), 'b', linewidth=1);
#plt.plot(r_vals, np.asarray(scf_vals_balabolka_man8x), 'r', linewidth=1.0);
#plt.plot(r_vals, np.asarray(scf_vals_pad_03), 'g', linewidth=1.0);

# logarithmic y-axis
#plt.yscale('log');
#plt.gca().minorticks_off();
         

plt.title(r"The rate of negative space reduction");
plt.show();
