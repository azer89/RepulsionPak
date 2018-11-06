import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

# parameters
d_gap = 5.5;
h_step = 0.1; # gap
max_val = 20;
neg_space_ratio = 0.5; # negative space ration 0..1.0

r_vals = np.arange(0.0, max_val, h_step );

first_len = d_gap / h_step;
first_array = np.linspace(neg_space_ratio, 0, first_len);
#first_array = np.arange(0, d_gap + h_step, h_step);
second_array = np.zeros(len(r_vals) - len(first_array) );
#second_array = np.zeros(d_gap + h_step, max_val, h_step);

tri_array = np.concatenate((first_array, second_array), axis = 0);


figfig = plt.figure(1);
figfig.patch.set_facecolor('white');
plt.clf();
ax = plt.gca();

plt.plot(r_vals, tri_array, 'r', linewidth=2);

plt.title(r"Test");
plt.show();
