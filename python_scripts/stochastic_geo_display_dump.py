import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
#import pandas as pd

import useful_things as ut

##################
# h_gap and r_vals
##################
h_gap = 0.1;
r_vals = np.arange(h_gap, 40, h_gap);
r_vals = np.insert(r_vals, 0, 0);
r_vals = np.insert(r_vals, 0, 0);
########
# figure
########
fig1 = plt.figure(1);
fig1.patch.set_facecolor('white');
plt.clf();
ax = plt.gca();
ax.set_xlim(0, 10);

"""
# 1000
num_folder = "";
case_name = "1000";  # case
num_names = ["0", "1", "2", "3", "4"];

"""
for num_name in num_names:
    str_cmd = "data_array = " "scf_vals_" + case_name + "_" + num_name;
    print str_cmd;
    exec(str_cmd);
    
    data_array = [x for x in data_array if x != 0];
    idx_array = r_vals[:len(data_array)];
    
    plt.plot(idx_array, np.asarray(data_array), 'r', linewidth=1);
"""
"""
# 250
case_name = "250";  # case
for num_name in num_names:
    str_cmd = "data_array = " "scf_vals_" + case_name + "_" + num_name;
    print str_cmd;
    exec(str_cmd);
    
    data_array = [x for x in data_array if x != 0];
    idx_array = r_vals[:len(data_array)];
    
    plt.plot(idx_array, np.asarray(data_array), 'g', linewidth=1);
"""


"""
case_name = "5";  # case
for num_name in num_names:
    str_cmd = "data_array = " "scf_vals_" + case_name + "_" + num_name;
    print str_cmd;
    exec(str_cmd);
    
    data_array = [x for x in data_array if x != 0];
    idx_array = r_vals[:len(data_array)];
    
    plt.plot(idx_array, np.asarray(data_array), 'b', linewidth=1);


plt.title(r"SCPs of all");

plt.show();
