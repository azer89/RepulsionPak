import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

r_vals = np.arange(0.2, 21, 0.1);
r_vals = np.insert(r_vals, 0, 0);


fig3 = plt.figure(3);
fig3.patch.set_facecolor('white');
plt.clf();
ax = plt.gca();

green_array = scf_vals_pad_01;
red_array = scf_vals_pad_man2x;



#blue_array = [x for x in blue_array if x != 0]
green_array = [x for x in green_array if x != 0];
red_array = [x for x in red_array if x != 0];

max_green = max(green_array);
max_red = max(red_array);

green_array = [max_green - x for x in green_array];
red_array = [max_red - x for x in red_array];


green_idx = r_vals[:len(green_array)];
red_idx = r_vals[:len(red_array)];

plt.plot(red_idx, np.asarray(red_array), 'r', linewidth=2);
plt.plot(green_idx, np.asarray(green_array), 'g', linewidth=2);

#plt.plot([5.746, 5.746], [0, 0.33], 'k-')

#plt.plot(r_vals, np.asarray(scf_vals_boxes_e), 'b', linewidth=1);
#plt.plot(r_vals, np.asarray(scf_vals_balabolka_man8x), 'r', linewidth=1.0);
#plt.plot(r_vals, np.asarray(scf_vals_pad_03), 'g', linewidth=1.0);

# logarithmic y-axis
#plt.yscale('log');
#plt.gca().invert_yaxis();
#plt.gca().minorticks_off();
         

plt.title(r"PAD (red) vs RepulsionPak (green). Log scale");
plt.show();

